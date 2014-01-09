#include "AugmentedRealityApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"

namespace MagicApp
{
    AugmentedRealityApp::AugmentedRealityApp() :
        mpVideoCanvas(NULL),
        mpVCTex(NULL),
        mpVCMat(NULL),
        mTexWidth(1024),
        mTexHeight(1024),
        mVideoCapture(),
        mIsUpdateVideoCanvas(false),
        mVideoWidth(1),
        mVideoHeight(1),
        mOneFrameTime(0),
        mTimeAccumulate(0),
        mCurrentFrame(0),
        mFrameCount(0),
        mIsCaptureFrame(false),
        mDisplayIndex(0),
        mImageNeedNumber(8)
    {
    }

    AugmentedRealityApp::~AugmentedRealityApp()
    {
    }

    bool AugmentedRealityApp::Enter(void)
    {
        InfoLog << "Enter AugmentedRealityApp" << std::endl; 
        mUI.Setup();
        SetupScene();
        return true;
    }

    bool AugmentedRealityApp::Update(float timeElapsed)
    {
        if (mIsUpdateVideoCanvas)
        {
            //DebugLog << "mTimeAccumulate: " << mTimeAccumulate << " timeElapsed: " << timeElapsed << std::endl; 
            if (timeElapsed > mOneFrameTime * 2)
            {
                return true;
            }
            mTimeAccumulate += timeElapsed;
            while (mTimeAccumulate > mOneFrameTime)
            {
                UpdateCanvas();
                mTimeAccumulate -= mOneFrameTime;
            }
        }
        return true;
    }
    
    bool AugmentedRealityApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();
        ResetParameter();
        return true;
    }

    bool AugmentedRealityApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        return true;
    }

    bool AugmentedRealityApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool AugmentedRealityApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool AugmentedRealityApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_D)
        {
            DetectCorners();
        }
        return true;
    }

    void AugmentedRealityApp::WindowResized( Ogre::RenderWindow* rw )
    {
        int winW = rw->getWidth();
        int winH = rw->getHeight();
        UpdateCanvasSize(winW, winH, mVideoWidth, mVideoHeight);
    }

    void AugmentedRealityApp::SetupScene(void)
    {
        InfoLog << "AugmentedRealityApp::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light* sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(0, 0, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);

        //Setup Video Canvas
        mpVCTex = Ogre::TextureManager::getSingleton().createManual(
            "VCTex",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::TEX_TYPE_2D,
            mTexWidth,
            mTexHeight,
            0,
            Ogre::PF_B8G8R8A8,
            Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE
            );
        Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
        unsigned char* pVCBuffer = static_cast<unsigned char*>(
            pVCPixelBuffer->lock(0, mTexWidth * mTexHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
        for (int yid = 0; yid < mTexHeight; yid++)
        {
            for (int xid = 0; xid < mTexWidth; xid++)
            {
                pVCBuffer[ (yid * mTexWidth + xid) * 4 + 0] = 220;
                pVCBuffer[ (yid * mTexWidth + xid) * 4 + 1] = 220;
                pVCBuffer[ (yid * mTexWidth + xid) * 4 + 2] = 220;
                pVCBuffer[ (yid * mTexWidth + xid) * 4 + 3] = 255;
            }
        }
        pVCPixelBuffer->unlock();

        mpVCMat = Ogre::MaterialManager::getSingleton().create(
            "VCMat",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        mpVCMat->getTechnique(0)->getPass(0)->createTextureUnitState("VCTex");
        mpVCMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);

        mpVideoCanvas = new Ogre::Rectangle2D(true);
        mpVideoCanvas->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
        mpVideoCanvas->setMaterial("VCMat");
        Ogre::RenderWindow* rw = MagicCore::RenderSystem::GetSingleton()->GetRenderWindow();
        int winW = rw->getWidth();
        int winH = rw->getHeight();
        UpdateCanvasSize(winW, winH, mVideoWidth, mVideoHeight);

        if (pSceneMgr->hasSceneNode("ModelNode"))
        {
            pSceneMgr->getSceneNode("ModelNode")->attachObject(mpVideoCanvas);
        }
        else
        {
            pSceneMgr->getRootSceneNode()->createChildSceneNode("ModelNode")->attachObject(mpVideoCanvas);
        }
    }

    void AugmentedRealityApp::ShutdownScene(void)
    {
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        if (pSceneMgr->hasSceneNode("ModelNode"))
        {
            pSceneMgr->getSceneNode("ModelNode")->detachObject(mpVideoCanvas);
            if (mpVideoCanvas != NULL)
            {
                delete mpVideoCanvas;
                mpVideoCanvas = NULL;
            }
        }
        Ogre::MaterialManager::getSingleton().remove("VCMat");
        mpVCMat.setNull();
        Ogre::TextureManager::getSingleton().remove("VCTex");
        mpVCTex.setNull();
        mVideoCapture.release();
    }

    void AugmentedRealityApp::ResetParameter(void)
    {
        mIsUpdateVideoCanvas = false;
        mVideoWidth = 1;
        mVideoHeight = 1;
        mOneFrameTime = 0;
        mTimeAccumulate = 0;
        mCurrentFrame = 0;
        mFrameCount = 0;
        mIsCaptureFrame = false;
        mDisplayIndex = 0;
        mSelectedImages.clear();
        mSelectedIndex.clear();
    }

    void AugmentedRealityApp::UpdateImageToCanvas(cv::Mat& image)
    {
        cv::Size vcSize(mTexWidth, mTexHeight);
        cv::Mat newFrame(vcSize, CV_8UC3);
        cv::resize(image, newFrame, vcSize);
        int nChannel = newFrame.channels();
        //DebugLog << "nChannel: " << nChannel << std::endl;
        if (nChannel == 1)
        {
            Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
            unsigned char* pVCBuffer = static_cast<unsigned char*>(
                pVCPixelBuffer->lock(0, mTexWidth * mTexHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
            for (int yid = 0; yid < mTexHeight; yid++)
            {
                for (int xid = 0; xid < mTexWidth; xid++)
                {
                    unsigned char* pixel = newFrame.ptr(xid, yid);
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 0] = pixel[0];
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 1] = pixel[0];
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 2] = pixel[0];
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 3] = 255;
                }
            }
            pVCPixelBuffer->unlock();
        }
        else if (nChannel == 3)
        {
            Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
            unsigned char* pVCBuffer = static_cast<unsigned char*>(
                pVCPixelBuffer->lock(0, mTexWidth * mTexHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
            for (int yid = 0; yid < mTexHeight; yid++)
            {
                for (int xid = 0; xid < mTexWidth; xid++)
                {
                    unsigned char* pixel = newFrame.ptr(yid, xid);
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 0] = pixel[0];
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 1] = pixel[1];
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 2] = pixel[2];
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 3] = 255;
                }
            }
            pVCPixelBuffer->unlock();
        }
        else if (nChannel >= 4)
        {
            Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
            unsigned char* pVCBuffer = static_cast<unsigned char*>(
                pVCPixelBuffer->lock(0, mTexWidth * mTexHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
            for (int yid = 0; yid < mTexHeight; yid++)
            {
                for (int xid = 0; xid < mTexWidth; xid++)
                {
                    unsigned char* pixel = newFrame.ptr(xid, yid);
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 0] = pixel[0];
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 1] = pixel[1];
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 2] = pixel[2];
                    pVCBuffer[ (yid * mTexWidth + xid) * 4 + 3] = pixel[3];
                }
            }
            pVCPixelBuffer->unlock();
        }
    }

    void AugmentedRealityApp::UpdateCanvas(void)
    {
        cv::Mat newFrameOrigin;
        //mVideoCapture >> newFrameOrigin;
        if (mVideoCapture.read(newFrameOrigin) == false)
        {
            DebugLog << "Video Begin Again" << std::endl;
            mVideoCapture.set(CV_CAP_PROP_POS_FRAMES, 0);
            mVideoCapture.read(newFrameOrigin);
            mCurrentFrame = 0;
        }
        UpdateImageToCanvas(newFrameOrigin);
        //
        if (mIsCaptureFrame)
        {
            DebugLog << "Capture a Frame" << std::endl;
            mSelectedImages.push_back(newFrameOrigin);
            mSelectedIndex.push_back(mCurrentFrame);
            mIsCaptureFrame = false;
            if (mSelectedImages.size() == mImageNeedNumber)
            {
                mDisplayIndex = 0;
                UpdateImageToCanvas(mSelectedImages.at(mDisplayIndex));
                mIsUpdateVideoCanvas = false;
                mUI.SetupImageBrowsing();
                mUI.SetSliderPosition(mSelectedIndex.at(mDisplayIndex));
            }
        }
        mUI.SetSliderPosition(mCurrentFrame);
        mCurrentFrame++;
        //
    }

    void AugmentedRealityApp::UpdateCanvasSize(int winW, int winH, int videoW, int videoH)
    {
        float canvasW, canvasH;
        float maxSize = 0.7;
        if (videoW > videoH)
        {
            canvasW = maxSize;
            canvasH = maxSize * videoH / videoW;
        }
        else
        {
            canvasH = maxSize;
            canvasW = maxSize * videoW / videoH;
        }
        if (winW > winH)
        {
            //canvasW *= float(winH) / float(winW);
            canvasH *= float(winW) / float(winH);
            if (canvasH > maxSize)
            {
                canvasW *= maxSize / canvasH;
                canvasH = maxSize;
            }
        }
        else
        {
            //canvasH *= float(winW) / float(winH);
            canvasW *= float(winH) / float(winW);
            if (canvasW > maxSize)
            {
                canvasH *= maxSize / canvasW;
                canvasW = maxSize;
            }
        }
        mpVideoCanvas->setCorners(-canvasW, canvasH, canvasW, -canvasH);
    }

    bool AugmentedRealityApp::OpenVideo()
    {
        std::string fileName;
        char filterName[] = "AVI Files(*.avi)\0*.avi\0All Files(*.*)\0*.*\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            if (mVideoCapture.open(fileName))
            {
                DebugLog << "Open video: " << fileName.c_str() << std::endl;
                ResetParameter();
                mIsUpdateVideoCanvas = true;
                int frameRate = mVideoCapture.get(CV_CAP_PROP_FPS);
                mOneFrameTime = 1.f / frameRate;
                mTimeAccumulate = 0.f;
                mVideoWidth = mVideoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
                mVideoHeight = mVideoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
                mCurrentFrame = mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);
                mFrameCount = mVideoCapture.get(CV_CAP_PROP_FRAME_COUNT);
                DebugLog << "fps: " << frameRate << " frameCount: " << mFrameCount << std::endl;
                mUI.SetSliderRange(mFrameCount);
                mUI.SetSliderPosition(mCurrentFrame);
                Ogre::RenderWindow* rw = MagicCore::RenderSystem::GetSingleton()->GetRenderWindow();
                int winW = rw->getWidth();
                int winH = rw->getHeight();
                UpdateCanvasSize(winW, winH, mVideoWidth, mVideoHeight);
                return true;
            }
            else
            {
                DebugLog << "Fail to open " << fileName.c_str() << std::endl;
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    void AugmentedRealityApp::SetFramePosition(int pos)
    {
        mCurrentFrame = pos;
        mVideoCapture.set(CV_CAP_PROP_POS_FRAMES, pos);
    }

    void AugmentedRealityApp::CaptureFrame()
    {
        mIsCaptureFrame = true;
    }

    void AugmentedRealityApp::ClearCapture()
    {
        mSelectedImages.clear();
        mSelectedIndex.clear();
    }

    void AugmentedRealityApp::NextImage()
    {
        mDisplayIndex++;
        if (mDisplayIndex == mImageNeedNumber)
        {
            mDisplayIndex = 0;
        }
        UpdateImageToCanvas( mSelectedImages.at(mDisplayIndex) );
        mUI.SetSliderPosition(mSelectedIndex.at(mDisplayIndex));
    }

    void AugmentedRealityApp::DetectCorners()
    {
        std::vector<cv::Point2f> imageCorners;
        cv::Size boardSize(4,4);
        bool found = cv::findChessboardCorners(mSelectedImages.at(mDisplayIndex), boardSize, imageCorners);
        if (found)
        {
            DebugLog << "Find corners" << std::endl;
            cv::drawChessboardCorners(mSelectedImages.at(mDisplayIndex), boardSize, imageCorners, found);
            UpdateImageToCanvas( mSelectedImages.at(mDisplayIndex) );
        }
        else
        {
            DebugLog << "Can not find corners" << std::endl;
        }
    }
}
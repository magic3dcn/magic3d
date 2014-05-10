#include "stdafx.h"
#include "AugmentedRealityApp.h"
#include "../Tool/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"

namespace MagicApp
{
    AugmentedRealityApp::AugmentedRealityApp() :
        mpVCTex(NULL),
        mpVCMat(NULL),
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
        mImageNeedNumber(3),
        mCameraFov(1.570796326794897f),
        mpBackboardMesh(NULL),
        mBackboardDepth(100.f)
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
        if (arg.key == OIS::KC_T)
        {
            GetValidImages();
        }
        return true;
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
    }

    void AugmentedRealityApp::SetupBackboard()
    {
        //Setup Video Canvas
        if (!mpVCTex.isNull())
        {
            Ogre::TextureManager::getSingleton().remove("VCTex");
            mpVCTex.setNull();
        }
        mpVCTex = Ogre::TextureManager::getSingleton().createManual(
            "VCTex",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::TEX_TYPE_2D,
            mVideoWidth,
            mVideoHeight,
            0,
            Ogre::PF_B8G8R8A8,
            Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE
            );
        Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
        unsigned char* pVCBuffer = static_cast<unsigned char*>(
            pVCPixelBuffer->lock(0, mVideoWidth * mVideoHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
        for (int yid = 0; yid < mVideoHeight; yid++)
        {
            for (int xid = 0; xid < mVideoWidth; xid++)
            {
                pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 0] = 220;
                pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 1] = 220;
                pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 2] = 220;
                pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 3] = 255;
            }
        }
        pVCPixelBuffer->unlock();

        if (!mpVCMat.isNull())
        {
            Ogre::MaterialManager::getSingleton().remove("VCMat");
            mpVCMat.setNull();
        }
        mpVCMat = Ogre::MaterialManager::getSingleton().create(
            "VCMat",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        mpVCMat->getTechnique(0)->getPass(0)->createTextureUnitState("VCTex");
        mpVCMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);

        UpdateCameraParm();
    }

    void AugmentedRealityApp::UpdateCameraParm()
    {
        Ogre::Camera* pCam = MagicCore::RenderSystem::GetSingleton()->GetMainCamera();
        pCam->setFOVy(Ogre::Radian(mCameraFov));
        //pCam->setAspectRatio(640.f / 480.f);
        pCam->setPosition(0, 0, 0);

        float boardHalfHeight = mBackboardDepth * tan(mCameraFov / 2.f);
        float boardHalfWidth = boardHalfHeight * mVideoWidth / mVideoHeight;

        if (mpBackboardMesh == NULL)
        {
            mpBackboardMesh = new MagicDGP::LightMesh3D;
            mpBackboardMesh->InsertVertex(MagicMath::Vector3(-boardHalfWidth, -boardHalfHeight, -mBackboardDepth));
            mpBackboardMesh->InsertVertex(MagicMath::Vector3(boardHalfWidth, -boardHalfHeight, -mBackboardDepth));
            mpBackboardMesh->InsertVertex(MagicMath::Vector3(boardHalfWidth, boardHalfHeight, -mBackboardDepth));
            mpBackboardMesh->InsertVertex(MagicMath::Vector3(-boardHalfWidth, boardHalfHeight, -mBackboardDepth));
            MagicDGP::FaceIndex fi;
            fi.mIndex[0] = 0;
            fi.mIndex[1] = 1;
            fi.mIndex[2] = 3;
            mpBackboardMesh->InsertFace(fi);
            fi.mIndex[0] = 1;
            fi.mIndex[1] = 2;
            fi.mIndex[2] = 3;
            mpBackboardMesh->InsertFace(fi);
            mpBackboardMesh->UpdateNormal();
            mpBackboardMesh->GetVertex(0)->SetTexCord(MagicMath::Vector3(0, 1, 0));
            mpBackboardMesh->GetVertex(1)->SetTexCord(MagicMath::Vector3(1, 1, 0));
            mpBackboardMesh->GetVertex(2)->SetTexCord(MagicMath::Vector3(1, 0, 0));
            mpBackboardMesh->GetVertex(3)->SetTexCord(MagicMath::Vector3(0, 0, 0));
        }
        else
        {
            mpBackboardMesh->GetVertex(0)->SetPosition(MagicMath::Vector3(-boardHalfWidth, -boardHalfHeight, -mBackboardDepth));
            mpBackboardMesh->GetVertex(1)->SetPosition(MagicMath::Vector3(boardHalfWidth, -boardHalfHeight, -mBackboardDepth));
            mpBackboardMesh->GetVertex(2)->SetPosition(MagicMath::Vector3(boardHalfWidth, boardHalfHeight, -mBackboardDepth));
            mpBackboardMesh->GetVertex(3)->SetPosition(MagicMath::Vector3(-boardHalfWidth, boardHalfHeight, -mBackboardDepth));
        }
        MagicCore::RenderSystem::GetSingleton()->RenderLightMesh3DWithTexture("BackBoard", "VCMat", mpBackboardMesh);
    }

    void AugmentedRealityApp::ShutdownScene(void)
    {
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("BackBoard");

        if (!mpVCMat.isNull())
        {
            Ogre::MaterialManager::getSingleton().remove("VCMat");
            mpVCMat.setNull();
        }
        if (!mpVCTex.isNull())
        {
            Ogre::TextureManager::getSingleton().remove("VCTex");
            mpVCTex.setNull();
        }
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
        cv::Size vcSize(mVideoWidth, mVideoHeight);
        cv::Mat newFrame(vcSize, CV_8UC3);
        cv::resize(image, newFrame, vcSize);
        int nChannel = newFrame.channels();
        //DebugLog << "nChannel: " << nChannel << std::endl;
        if (nChannel == 1)
        {
            Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
            unsigned char* pVCBuffer = static_cast<unsigned char*>(
                pVCPixelBuffer->lock(0, mVideoWidth * mVideoHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
            for (int yid = 0; yid < mVideoHeight; yid++)
            {
                for (int xid = 0; xid < mVideoWidth; xid++)
                {
                    unsigned char* pixel = newFrame.ptr(xid, yid);
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 0] = pixel[0];
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 1] = pixel[0];
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 2] = pixel[0];
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 3] = 255;
                }
            }
            pVCPixelBuffer->unlock();
        }
        else if (nChannel == 3)
        {
            Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
            unsigned char* pVCBuffer = static_cast<unsigned char*>(
                pVCPixelBuffer->lock(0, mVideoWidth * mVideoHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
            for (int yid = 0; yid < mVideoHeight; yid++)
            {
                for (int xid = 0; xid < mVideoWidth; xid++)
                {
                    unsigned char* pixel = newFrame.ptr(yid, xid);
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 0] = pixel[0];
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 1] = pixel[1];
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 2] = pixel[2];
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 3] = 255;
                }
            }
            pVCPixelBuffer->unlock();
        }
        else if (nChannel >= 4)
        {
            Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
            unsigned char* pVCBuffer = static_cast<unsigned char*>(
                pVCPixelBuffer->lock(0, mVideoWidth * mVideoHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
            for (int yid = 0; yid < mVideoHeight; yid++)
            {
                for (int xid = 0; xid < mVideoWidth; xid++)
                {
                    unsigned char* pixel = newFrame.ptr(xid, yid);
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 0] = pixel[0];
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 1] = pixel[1];
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 2] = pixel[2];
                    pVCBuffer[ (yid * mVideoWidth + xid) * 4 + 3] = pixel[3];
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
                SetupBackboard();
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

    void AugmentedRealityApp::GetValidImages()
    {
        DebugLog << "GetValidImages" << std::endl;
        std::vector<bool> visitFlag(mFrameCount, 0);
        int delta = 1;
        int currentFrame = 0; 
        std::vector<std::vector<cv::Point3f> > objectPoints;
        std::vector<cv::Point3f> objectCorners;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                objectCorners.push_back(cv::Point3f(i, j, 0.0f));
            }
        }
        std::vector<std::vector<cv::Point2f> > imagePoints;
        while(1)
        {
            if (!(visitFlag.at(currentFrame)))
            {
                visitFlag.at(currentFrame) = 1;
                std::vector<cv::Point2f> imageCorners;
                cv::Size boardSize(4, 4);
                cv::Mat newFrame;
                mVideoCapture.read(newFrame);
                bool found = cv::findChessboardCorners(newFrame, boardSize, imageCorners);
                if (found)
                {
                    /*cv::cornerSubPix(newFrame, imageCorners, cv::Size(5, 5), cv::Size(-1,-1), cv::TermCriteria(cv::TermCriteria::MAX_ITER +
                        cv::TermCriteria::EPS, 30, 0.1));*/ 
                    cv::drawChessboardCorners(newFrame, boardSize, imageCorners, found);
                    mSelectedImages.push_back(newFrame);
                    mSelectedIndex.push_back(currentFrame);
                    imagePoints.push_back(imageCorners);
                    objectPoints.push_back(objectCorners);
                    DebugLog << "Find valid image: " << mSelectedImages.size() << std::endl;
                }
                if (mSelectedImages.size() == mImageNeedNumber)
                {
                    break;
                }
            }
            currentFrame += delta;
            currentFrame = currentFrame % mFrameCount;
        }
        //
        DebugLog << "calibreateCamera......" << std::endl;
        std::vector<cv::Mat> rvecs, tvecs;
        cv::Mat cameraMatrix;
        cv::Mat distCoeffs;
        int flag;
        cv::Size imageSize(640, 480);
        calibrateCamera(objectPoints, // the 3D points
              imagePoints, // the image points
              imageSize,   // image size
              cameraMatrix,// output camera matrix
              distCoeffs,  // output distortion matrix
              rvecs, tvecs,// Rs, Ts
              flag);       // set options
        cv::Mat map1,map2;
        DebugLog << "cv::initUndistortRectifyMap......" << std::endl;
        cv::initUndistortRectifyMap(
                cameraMatrix,  // computed camera matrix
                distCoeffs,    // computed distortion matrix
                cv::Mat(),     // optional rectification (none)
                cv::Mat(),     // camera matrix to generate undistorted
                imageSize,  // size of undistorted
                CV_32FC1,      // type of output map
                map1, map2);   // the x and y mapping functions
        DebugLog << "CameraMatrix: " << cameraMatrix.rows << " * " << cameraMatrix.cols << " type: " << cameraMatrix.type() << std::endl;
        for (int rid = 0; rid < cameraMatrix.rows; rid++)
        {
            for (int cid = 0; cid < cameraMatrix.cols; cid++)
            {
                DebugLog << cameraMatrix.at<double>(rid, cid) << " ";
            }
            DebugLog << std::endl;
        }
        DebugLog << "Rectify selected images" << std::endl;
        for (int i = 0; i < mSelectedImages.size(); i++)
        {
            cv::Mat undistorted;
            cv::remap(mSelectedImages.at(i), undistorted, map1, map2, cv::INTER_LINEAR);
            mSelectedImages.at(i) = undistorted;
        }
        DebugLog << "SetUp UI" << std::endl;
        mDisplayIndex = 0;
        mIsUpdateVideoCanvas = false;
        mUI.SetupImageBrowsing();
        mUI.SetSliderPosition(mSelectedIndex.at(mDisplayIndex));

    }
}
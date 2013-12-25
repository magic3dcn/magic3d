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
        mVCWidth(512),
        mVCHeight(512),
        mVideoCapture(),
        mIsUpdateVideoCanvas(false)
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
            UpdateCanvas();
        }
        return true;
    }
    
    bool AugmentedRealityApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();
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
        return true;
    }

    void AugmentedRealityApp::WindowResized( Ogre::RenderWindow* rw )
    {

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
            mVCWidth,
            mVCHeight,
            0,
            Ogre::PF_B8G8R8A8,
            Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE
            );
        Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
        unsigned char* pVCBuffer = static_cast<unsigned char*>(
            pVCPixelBuffer->lock(0, mVCWidth * mVCHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
        for (int yid = 0; yid < mVCHeight; yid++)
        {
            for (int xid = 0; xid < mVCWidth; xid++)
            {
                pVCBuffer[ (yid * mVCWidth + xid) * 4 + 0] = 255;
                pVCBuffer[ (yid * mVCWidth + xid) * 4 + 1] = 0;
                pVCBuffer[ (yid * mVCWidth + xid) * 4 + 2] = 0;
                pVCBuffer[ (yid * mVCWidth + xid) * 4 + 3] = 255;
            }
        }
        pVCPixelBuffer->unlock();

        mpVCMat = Ogre::MaterialManager::getSingleton().create(
            "VCMat",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        mpVCMat->getTechnique(0)->getPass(0)->createTextureUnitState("VCTex");
        mpVCMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);

        mpVideoCanvas = new Ogre::Rectangle2D(true);
        mpVideoCanvas->setCorners(-0.8, 0.6, 0.8, -0.6);
        mpVideoCanvas->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
        mpVideoCanvas->setMaterial("VCMat");

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

    }

    void AugmentedRealityApp::UpdateCanvas(void)
    {
        cv::Mat newFrameOrigin;
        mVideoCapture >> newFrameOrigin;
        cv::Size vcSize(mVCWidth, mVCHeight);
        cv::Mat newFrame(vcSize, CV_8UC3);
        cv::resize(newFrameOrigin, newFrame, vcSize);
        /*cv::Mat newFrame;
        mVideoCapture >> newFrame;*/
        int nChannel = newFrame.channels();
        DebugLog << "nChannel: " << nChannel << std::endl;
        if (nChannel == 1)
        {
            Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
            unsigned char* pVCBuffer = static_cast<unsigned char*>(
                pVCPixelBuffer->lock(0, mVCWidth * mVCHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
            for (int yid = 0; yid < mVCHeight; yid++)
            {
                for (int xid = 0; xid < mVCWidth; xid++)
                {
                    unsigned char* pixel = newFrame.ptr(xid, yid);
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 0] = pixel[0];
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 1] = pixel[0];
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 2] = pixel[0];
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 3] = 255;
                }
            }
            pVCPixelBuffer->unlock();
        }
        else if (nChannel == 3)
        {
            Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
            unsigned char* pVCBuffer = static_cast<unsigned char*>(
                pVCPixelBuffer->lock(0, mVCWidth * mVCHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
            for (int yid = 0; yid < mVCHeight; yid++)
            {
                for (int xid = 0; xid < mVCWidth; xid++)
                {
                    unsigned char* pixel = newFrame.ptr(yid, xid);
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 0] = pixel[0];
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 1] = pixel[1];
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 2] = pixel[2];
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 3] = 255;
                }
            }
            pVCPixelBuffer->unlock();
        }
        else if (nChannel >= 4)
        {
            Ogre::HardwarePixelBufferSharedPtr pVCPixelBuffer = mpVCTex->getBuffer();
            unsigned char* pVCBuffer = static_cast<unsigned char*>(
                pVCPixelBuffer->lock(0, mVCWidth * mVCHeight * 4, Ogre::HardwareBuffer::HBL_DISCARD) );
            for (int yid = 0; yid < mVCHeight; yid++)
            {
                for (int xid = 0; xid < mVCWidth; xid++)
                {
                    unsigned char* pixel = newFrame.ptr(xid, yid);
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 0] = pixel[0];
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 1] = pixel[1];
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 2] = pixel[2];
                    pVCBuffer[ (yid * mVCWidth + xid) * 4 + 3] = pixel[3];
                }
            }
            pVCPixelBuffer->unlock();
        }
    }

    void AugmentedRealityApp::UpdateCanvasSize(int winW, int winH, int videoW, int videoH)
    {
        float canvasW, canvasH;
        if (videoW > videoH)
        {
            canvasW = 0.8f;
            canvasH = 0.8f * videoH / videoW;
        }
        else
        {
            canvasH = 0.8f;
            canvasW = 0.8f * videoW / videoH;
        }
        if (winW > winH)
        {
            canvasW *= float(videoH) / float(videoW);
        }
        else
        {
            canvasH *= float(videoW) / float(videoH);
        }
        mpVideoCanvas->setCorners(-canvasW, canvasH, canvasW, -canvasH);
    }

    bool AugmentedRealityApp::OpenVideo()
    {
        std::string fileName;
        char filterName[] = "AVI Files(*.avi)\0*.avi\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            if (mVideoCapture.open(fileName))
            {
                mIsUpdateVideoCanvas = true;
                return true;
            }
        }
        else
        {
            return false;
        }
    }
}
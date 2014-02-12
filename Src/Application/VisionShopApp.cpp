#include "stdafx.h"
#include "VisionShopApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DIP/Retargetting.h"

namespace MagicApp
{
    VisionShopApp::VisionShopApp() :
        mUI()
    {
    }

    VisionShopApp::~VisionShopApp()
    {
        mImage.release();
    }

    bool VisionShopApp::Enter(void)
    {
        InfoLog << "Enter VisionShopApp" << std::endl;
        mUI.Setup();
        SetupScene();

        return true;
    }

    bool VisionShopApp::Update(float timeElapsed)
    {
        return true;
    }

    bool VisionShopApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();

        return true;
    }

    bool VisionShopApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        return true;
    }

    bool VisionShopApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool VisionShopApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool VisionShopApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        return true;
    }

    void VisionShopApp::SetupScene(void)
    {

    }

    void VisionShopApp::ShutdownScene(void)
    {
        mImage.release();
    }

    bool VisionShopApp::OpenImage(int& w, int& h)
    {
        std::string fileName;
        char filterName[] = "Image Files(*.*)\0*.*\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mImage.release();
            mImage = cv::imread(fileName);
            if (mImage.data != NULL)
            {
                w = mImage.cols;
                h = mImage.rows;
                mUI.UpdateImageTexture(mImage);
                return true;
            }
        }
        return false;
    }

    void VisionShopApp::ImageResizing(int w, int h)
    {
        double startTime = MagicCore::ToolKit::GetTime();
        cv::Mat resizedImg = MagicDIP::Retargetting::SeamCarvingResizing(mImage, w, h);
        DebugLog << "ImageResizing time: " << MagicCore::ToolKit::GetTime() - startTime << std::endl;
        mUI.UpdateImageTexture(resizedImg);
    }
}

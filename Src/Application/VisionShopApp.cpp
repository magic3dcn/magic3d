#include "stdafx.h"
#include "VisionShopApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DIP/Retargetting.h"
#include "../DIP/Saliency.h"

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

    void VisionShopApp::WindowResized( Ogre::RenderWindow* rw )
    {
        if (mImage.data != NULL)
        {
            cv::Mat resizedImg = ResizeToViewSuit(mImage);
            mUI.UpdateImageTexture(resizedImg);
        }
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
                cv::Mat resizedImg = ResizeToViewSuit(mImage);
                mUI.UpdateImageTexture(resizedImg);
                return true;
            }
        }
        return false;
    }

    void VisionShopApp::SaveImage()
    {
        std::string fileName;
        char filterName[] = "Support format(*.jpg, *.png, *.bmp, *tif)\0*.*\0";
        if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
        {
            if (mImage.data != NULL)
            {
                cv::imwrite(fileName, mImage);
            }
        }
    }

    void VisionShopApp::ImageResizing(int w, int h)
    {
        double startTime = MagicCore::ToolKit::GetTime();
        cv::Mat resizedImg = MagicDIP::Retargetting::SeamCarvingResizing(mImage, w, h);
        mImage = resizedImg.clone();
        DebugLog << "ImageResizing time: " << MagicCore::ToolKit::GetTime() - startTime << std::endl;
        cv::Mat resizedToViewImg = ResizeToViewSuit(mImage);
        mUI.UpdateImageTexture(resizedToViewImg);
    }

    void VisionShopApp::FastImageResizing(int w, int h)
    {
        double startTime = MagicCore::ToolKit::GetTime();
        //cv::Mat resizedImg = MagicDIP::Retargetting::FastSeamCarvingResizing(mImage, w, h);
        cv::Mat resizedImg = MagicDIP::Retargetting::SaliencyBasedSeamCarvingResizing(mImage, w, h);
        //cv::Mat resizedImg = MagicDIP::Retargetting::ImportanceDiffusionSeamCarvingResizing(mImage, w, h);
        mImage = resizedImg.clone();
        DebugLog << "ImageResizing time: " << MagicCore::ToolKit::GetTime() - startTime << std::endl;
        cv::Mat resizedToViewImg = ResizeToViewSuit(mImage);
        mUI.UpdateImageTexture(resizedToViewImg);
    }

    void VisionShopApp::SaliencyDetection(void)
    {
        //cv::Mat saliencyImg = MagicDIP::SaliencyDetection::DoGBandSaliency(mImage);
        //cv::Mat saliencyImg = MagicDIP::SaliencyDetection::GradientSaliency(mImage);
        cv::Mat saliencyImg = MagicDIP::SaliencyDetection::DoGAndGradientSaliency(mImage);
        mImage = saliencyImg.clone();
        cv::Mat resizedToViewImg = ResizeToViewSuit(saliencyImg);
        mUI.UpdateImageTexture(resizedToViewImg);
    }

    cv::Mat VisionShopApp::ResizeToViewSuit(const cv::Mat& img) const
    {
        int winW = MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getWidth() - 200;
        int winH = MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getHeight() - 20;
        int imgW = img.cols;
        int imgH = img.rows;
        bool resized = false;
        if (imgW > winW)
        {
            imgH = int(imgH * float(winW) / imgW);
            imgW = winW;
            resized = true;
        }
        if (imgH > winH)
        {
            imgW = int(imgW * float(winH) / imgH);
            imgH = winH;
            resized = true;
        }
        if (resized)
        {
            cv::Size vcSize(imgW, imgH);
            cv::Mat resizedImg(vcSize, CV_8UC3);
            cv::resize(img, resizedImg, vcSize);
            return resizedImg;
        }
        else
        {
            cv::Mat resizedImg = img.clone();
            return resizedImg;
        }
    }
}

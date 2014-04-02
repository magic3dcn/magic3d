#include "stdafx.h"
#include "VisionShopApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DIP/Retargetting.h"
#include "../DIP/Saliency.h"
#include "../DIP/Segmentation.h"
#include "../MachineLearning/Clustering.h"
#include "../MachineLearning/GaussianMixtureModel.h"

namespace MagicApp
{
    VisionShopApp::VisionShopApp() :
        mUI(),
        mMouseMode(MM_View)
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
        if (arg.state.buttonDown(OIS::MB_Left) )
        {
            int mouseWidth = 5;
            if (mMouseMode == MM_Paint_Front)
            {
                int wPos = arg.state.X.abs - 165;
                int hPos = arg.state.Y.abs - 10;
                for (int hid = -mouseWidth; hid <= mouseWidth; hid++)
                {
                    for (int wid = -mouseWidth; wid <= mouseWidth; wid++)
                    {
                        unsigned char* pixel = mMarkImage.ptr(hPos + hid, wPos + wid);
                        pixel[0] = 0;
                        pixel[1] = 0;
                        pixel[2] = 255;
                    }
                }
                mUI.UpdateMarkedImageTexture(mImage, mMarkImage);
            }
            else if (mMouseMode == MM_Paint_Back)
            {
                int wPos = arg.state.X.abs - 165;
                int hPos = arg.state.Y.abs - 10;
                for (int hid = -mouseWidth; hid <= mouseWidth; hid++)
                {
                    for (int wid = -mouseWidth; wid <= mouseWidth; wid++)
                    {
                        unsigned char* pixel = mMarkImage.ptr(hPos + hid, wPos + wid);
                        pixel[0] = 255;
                        pixel[1] = 0;
                        pixel[2] = 0;
                    }
                }
                mUI.UpdateMarkedImageTexture(mImage, mMarkImage);
            }
        }
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
                mImage = ResizeToViewSuit(mImage);
                mUI.UpdateImageTexture(mImage);
                w = mImage.cols;
                h = mImage.rows;
                //update brush image
                cv::Size imgSize(w, h);
                mMarkImage = cv::Mat(imgSize, CV_8UC3);
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
        //cv::Mat saliencyImg = MagicDIP::SaliencyDetection::DoGAndGradientSaliency(mImage);
        cv::Mat saliencyImg = MagicDIP::SaliencyDetection::MultiScaleDoGBandSaliency(mImage, 1, 1);
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

    void VisionShopApp::BrushFront()
    {
        mMouseMode = MM_Paint_Front;
    }

    void VisionShopApp::BrushBack()
    {
        mMouseMode = MM_Paint_Back;
    }

    void VisionShopApp::SegmentImageDo()
    {
        /*mMouseMode = MM_View;
        cv::Mat segImg = MagicDIP::Segmentation::SegmentByGraphCut(mImage, mMarkImage);
        mUI.UpdateMarkedImageTexture(mImage, segImg);*/
        
        //do an experiment about clustering
        int imgW = mImage.cols;
        int imgH = mImage.rows;
        int dim = 3;
        std::vector<double> sourceData;
        std::vector<double> inputData;
        for (int hid = 0; hid < imgH; hid++)
        {
            for (int wid = 0; wid < imgW; wid++)
            {
                unsigned char* pPixel = mImage.ptr(hid, wid);
                sourceData.push_back(pPixel[0]);
                sourceData.push_back(pPixel[1]);
                sourceData.push_back(pPixel[2]);
                inputData.push_back(pPixel[0]);
                inputData.push_back(pPixel[1]);
                inputData.push_back(pPixel[2]);
            }
        }
        std::vector<double> resData;
        MagicML::Clustering::MeanshiftValue(sourceData, dim, 5, inputData, resData);
        int pixelIndex = 0;
        for (int hid = 0; hid < imgH; hid++)
        {
            for (int wid = 0; wid < imgW; wid++)
            {
                unsigned char* pPixel = mImage.ptr(hid, wid);
                pPixel[0] = resData.at(pixelIndex * 3 + 0);
                pPixel[1] = resData.at(pixelIndex * 3 + 1);
                pPixel[2] = resData.at(pixelIndex * 3 + 2);
                pixelIndex++;
            }
        }
        mUI.UpdateImageTexture(mImage);
    }
}

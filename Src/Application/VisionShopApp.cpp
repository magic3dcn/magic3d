#include "stdafx.h"
#include "VisionShopApp.h"
#include "Tool/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "DIP/Retargetting.h"
#include "DIP/Saliency.h"
#include "DIP/Segmentation.h"
#include "MachineLearning/Clustering.h"
#include "MachineLearning/GaussianMixtureModel.h"
#include "DIP/Deformation.h"
#include "DIP/FeatureDetection.h"

namespace MagicApp
{
    VisionShopApp::VisionShopApp() :
        mUI(),
        mMouseMode(MM_View),
        mpPointSet(NULL),
        mIsPointSetMode(false),
        mIsNewImage(true),
        mIsDeformMarkSelected(false),
        mSelectedDeformMarkIndex(-1)
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
        SetDefaultParameter();

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
                int imgW = mMarkImage.cols;
                int imgH = mMarkImage.rows;
                int hBottom = hPos - mouseWidth;
                hBottom = hBottom > 0 ? hBottom : 0;
                int hUp = hPos + mouseWidth;
                hUp = hUp >= imgH ? imgH - 1 : hUp;
                int wLeft = wPos - mouseWidth;
                wLeft = wLeft > 0 ? wLeft : 0;
                int wRight = wPos + mouseWidth;
                wRight = wRight >= imgW ? imgW - 1 : wRight;
                for (int hid = hBottom; hid <= hUp; hid++)
                {
                    for (int wid = wLeft; wid <= wRight; wid++)
                    {
                        unsigned char* pixel = mMarkImage.ptr(hid, wid);
                        pixel[0] = 0;
                        pixel[1] = 0;
                        pixel[2] = 255;
                    }
                }
                UpdateDisplayImage(mMarkImage);
                Display();
            }
            else if (mMouseMode == MM_Paint_Back)
            {
                int wPos = arg.state.X.abs - 165;
                int hPos = arg.state.Y.abs - 10;
                int imgW = mMarkImage.cols;
                int imgH = mMarkImage.rows;
                int hBottom = hPos - mouseWidth;
                hBottom = hBottom > 0 ? hBottom : 0;
                int hUp = hPos + mouseWidth;
                hUp = hUp >= imgH ? imgH - 1 : hUp;
                int wLeft = wPos - mouseWidth;
                wLeft = wLeft > 0 ? wLeft : 0;
                int wRight = wPos + mouseWidth;
                wRight = wRight >= imgW ? imgW - 1 : wRight;
                for (int hid = hBottom; hid <= hUp; hid++)
                {
                    for (int wid = wLeft; wid <= wRight; wid++)
                    {
                        unsigned char* pixel = mMarkImage.ptr(hid, wid);
                        pixel[0] = 255;
                        pixel[1] = 0;
                        pixel[2] = 0;
                    }
                }
                UpdateDisplayImage(mMarkImage);
                Display();
            }
        }
        else if (arg.state.buttonDown(OIS::MB_Right))
        {
            if (mMouseMode == MM_Deformation && mIsDeformMarkSelected)
            {
                int wPos = arg.state.X.abs - 165;
                int hPos = arg.state.Y.abs - 10;
                int imgW = mMarkImage.cols;
                int imgH = mMarkImage.rows;
                if (wPos >= 0 && wPos < imgW && hPos >= 0 && hPos < imgH)
                {
                    std::vector<int> targetMarks = mDeformationMarks;
                    targetMarks.at(mSelectedDeformMarkIndex * 2) = wPos;
                    targetMarks.at(mSelectedDeformMarkIndex * 2 + 1) = hPos;
                    UpdateDisplayImage(targetMarks);
                    Display();
                }
            }
        }
        if (mIsPointSetMode)
        {
            MagicDGP::ViewTool::MouseMode mm;
            if (arg.state.buttonDown(OIS::MB_Left))
            {
                mm = MagicDGP::ViewTool::MM_Left_Down;
            }
            else if (arg.state.buttonDown(OIS::MB_Middle))
            {
                mm = MagicDGP::ViewTool::MM_Middle_Down;
            }
            else if (arg.state.buttonDown(OIS::MB_Right))
            {
                mm = MagicDGP::ViewTool::MM_Right_Down;
            }
            else
            {
                mm = MagicDGP::ViewTool::MM_None;
            }
            mViewTool.MouseMoved(arg.state.X.abs, arg.state.Y.abs, mm);
        }
        
        return true;
    }

    bool VisionShopApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mIsPointSetMode)
        {
            mViewTool.MousePressed(arg.state.X.abs, arg.state.Y.abs);
        }
        if (mMouseMode == MM_Deformation && id == OIS::MB_Right)
        {
            //Judge whether select
            int wPos = arg.state.X.abs - 165;
            int hPos = arg.state.Y.abs - 10;
            int tol = 5;
            mIsDeformMarkSelected = false;
            int markNum = mDeformationMarks.size() / 2;
            for (int mid = 0; mid < markNum; mid++)
            {
                if (abs(wPos - mDeformationMarks.at(2 * mid)) < tol &&
                    abs(hPos - mDeformationMarks.at(2 * mid + 1)) < tol)
                {
                    mIsDeformMarkSelected = true;
                    mSelectedDeformMarkIndex = mid;
                }
            }
        }
        
        return true;
    }

    bool VisionShopApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_Deformation)
        {
            if (id == OIS::MB_Left)
            {
                int wPos = arg.state.X.abs - 165;
                int hPos = arg.state.Y.abs - 10;
                mDeformationMarks.push_back(wPos);
                mDeformationMarks.push_back(hPos);
                UpdateDisplayImage(mDeformationMarks);
                Display();
            }
            else if (id == OIS::MB_Right && mIsDeformMarkSelected)
            {
                int wPos = arg.state.X.abs - 165;
                int hPos = arg.state.Y.abs - 10;
                int imgW = mMarkImage.cols;
                int imgH = mMarkImage.rows;
                if (wPos >= 0 && wPos < imgW && hPos >= 0 && hPos < imgH)
                {
                    std::vector<int> targetMarks = mDeformationMarks;
                    targetMarks.at(mSelectedDeformMarkIndex * 2) = wPos;
                    targetMarks.at(mSelectedDeformMarkIndex * 2 + 1) = hPos;
                    //Deform mImage
                    cv::Mat deformImg = MagicDIP::Deformation::DeformByMovingLeastSquares(mImage, mDeformationMarks, targetMarks);
                    mImage.release();
                    mImage = deformImg.clone();
                    //deformImg.release();
                    //
                    mDeformationMarks = targetMarks;
                    UpdateDisplayImage(mDeformationMarks);
                    Display();
                }
                else
                {
                    UpdateDisplayImage(mDeformationMarks);
                    Display();
                }
                mIsDeformMarkSelected = false;
                mSelectedDeformMarkIndex = -1;
            }
        }
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
            mImage.release();
            mImage = resizedImg.clone();
            mIsNewImage = true;
            UpdateAuxiliaryData();
            Display();
        }
    }

    void VisionShopApp::SetupScene(void)
    {
        InfoLog << "VisionShopApp::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light* sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(0, 0, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void VisionShopApp::ShutdownScene(void)
    {
        InfoLog << "PointShopApp::ShutdownScene" << std::endl;
        mImage.release();
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("RenderPointSet");
        if (MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasSceneNode("ModelNode"))
        {
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->resetToInitialState();
        } 
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
                cv::Mat resizedImg = ResizeToViewSuit(mImage);
                mImage.release();
                mImage = resizedImg.clone();
                SetDefaultParameter();
                //mIsNewImage = true;
                UpdateAuxiliaryData();
                Display();
                w = mImage.cols;
                h = mImage.rows;
                
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
            if (mDisplayImage.data != NULL)
            {
                cv::imwrite(fileName, mDisplayImage);
            }
        }
    }

    bool VisionShopApp::SwitchDisplayMode(void)
    {
        mIsPointSetMode = !mIsPointSetMode;
        Display();
        return mIsPointSetMode;
    }

    void VisionShopApp::UpdatePointSet(void)
    {
        if (mpPointSet != NULL)
        {
            delete mpPointSet;
            mpPointSet = NULL;
        }
        mpPointSet = new MagicDGP::Point3DSet;
        int imgW = mDisplayImage.cols;
        int imgH = mDisplayImage.rows;
        for (int hid = 0; hid < imgH; hid++)
        {
            for (int wid = 0; wid < imgW; wid++)
            {
                unsigned char* pixel = mDisplayImage.ptr(hid, wid);
                MagicMath::Vector3 pos(pixel[2], pixel[1], pixel[0]);
                MagicMath::Vector3 color(pixel[2] / 255.0, pixel[1] / 255.0, pixel[0] / 255.0);
                MagicDGP::Point3D* pPoint = new MagicDGP::Point3D(pos);
                pPoint->SetColor(color);
                mpPointSet->InsertPoint(pPoint);
            }
        }
        mpPointSet->UnifyPosition(2.0);
    }

    void VisionShopApp::UpdateDisplayImage(const cv::Mat& markImg)
    {
        int imgW = mImage.cols;
        int imgH = mImage.rows;
        cv::Size imgSize(imgW, imgH);
        mDisplayImage.release();
        mDisplayImage = cv::Mat(imgSize, CV_8UC3); 
        for (int hid = 0; hid < imgH; hid++)
        {
            for (int wid = 0; wid < imgW; wid++)
            {
                const unsigned char* pMark = markImg.ptr(hid, wid);
                unsigned char* pDisplay = mDisplayImage.ptr(hid, wid);
                if (pMark[0] > 0 || pMark[1] > 0 || pMark[2] > 0)
                {
                    pDisplay[0] = pMark[0];
                    pDisplay[1] = pMark[1];
                    pDisplay[2] = pMark[2];
                }
                else
                {
                    unsigned char* pImage = mImage.ptr(hid, wid);
                    pDisplay[0] = pImage[0];
                    pDisplay[1] = pImage[1];
                    pDisplay[2] = pImage[2];
                }
            }
        }
    }

    void VisionShopApp::UpdateDisplayImage(const std::vector<int>& markIndex)
    {
        mDisplayImage.release();
        mDisplayImage = mImage.clone();
        int imgW = mImage.cols;
        int imgH = mImage.rows;
        int markNum = markIndex.size() / 2;
        int markWidth = 3;
        for (int mid = 0; mid < markNum; mid++)
        {
            int wPos = markIndex.at(2 * mid);
            int hPos = markIndex.at(2 * mid + 1);
            int hBottom = hPos - markWidth;
            hBottom = hBottom > 0 ? hBottom : 0;
            int hUp = hPos + markWidth;
            hUp = hUp >= imgH ? imgH - 1 : hUp;
            int wLeft = wPos - markWidth;
            wLeft = wLeft > 0 ? wLeft : 0;
            int wRight = wPos + markWidth;
            wRight = wRight >= imgW ? imgW - 1 : wRight;
            for (int hid = hBottom; hid <= hUp; hid++)
            {
                for (int wid = wLeft; wid <= wRight; wid++)
                {
                    unsigned char* pixel = mDisplayImage.ptr(hid, wid);
                    pixel[0] = 0;
                    pixel[1] = 0;
                    pixel[2] = 255;
                }
            }
        }
    }

    void VisionShopApp::UpdateAuxiliaryData(void)
    {
        if (mIsNewImage) // Reset mMarkImage
        {
            int w = mImage.cols;
            int h = mImage.rows;
            cv::Size imgSize(w, h);
            mMarkImage.release();
            mMarkImage = cv::Mat(imgSize, CV_8UC3, cv::Scalar(0));
            mIsNewImage = false;
        }
        //update mDisplayImage
        UpdateDisplayImage(mMarkImage);
        //Update PointSet
        UpdatePointSet();
    }

    void VisionShopApp::Display()
    {
        if (mIsPointSetMode)
        {
            mUI.HideImageTexture();
            if (mpPointSet == NULL)
            {
                UpdatePointSet();
            }
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderPointSet", "SimplePointSize2", mpPointSet);
        }
        else
        {
            MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("RenderPointSet");
            mUI.UpdateImageTexture(mDisplayImage);
        }
    }

    void VisionShopApp::SetDefaultParameter(void)
    {
        mMouseMode = MM_View;
        mIsPointSetMode = false;
        mIsNewImage = true;
        mDeformationMarks.clear();
        mIsDeformMarkSelected = false;
        mSelectedDeformMarkIndex = -1;
    }

    void VisionShopApp::ImageResizing(int w, int h)
    {
        cv::Mat resizedImg = MagicDIP::Retargetting::SeamCarvingResizing(mImage, w, h);
        mImage.release();
        mImage = resizedImg.clone();
        mIsNewImage = true;
        resizedImg.release();
        UpdateAuxiliaryData();
        Display();
    }

    void VisionShopApp::FastImageResizing(int w, int h)
    {
        cv::Mat resizedImg = MagicDIP::Retargetting::FastSeamCarvingResizing(mImage, w, h);
        mImage.release();
        mImage = resizedImg.clone();
        mIsNewImage = true;
        resizedImg.release();
        UpdateAuxiliaryData();
        Display();
    }

    void VisionShopApp::ImageGradient(void)
    {
        cv::Mat saliencyImg = MagicDIP::SaliencyDetection::GradientSaliency(mImage);
        mImage.release();
        mImage = saliencyImg.clone();
        saliencyImg.release();
        UpdateAuxiliaryData();
        Display();
    }

    void VisionShopApp::CannyEdgeDetection(int threshold)
    {
        cv::Mat saliencyImg = MagicDIP::FeatureDetection::CannyEdgeDetection(mImage);
        mDisplayImage.release();
        mDisplayImage = saliencyImg.clone();
        saliencyImg.release();
        //use threshold
        int imgW = mDisplayImage.cols;
        int imgH = mDisplayImage.rows;
        for (int hid = 0; hid < imgH; hid++)
        {
            for (int wid = 0; wid < imgW; wid++)
            {
                if (mDisplayImage.ptr(hid, wid)[0] < threshold)
                {
                    unsigned char* pixel = mDisplayImage.ptr(hid, wid);
                    pixel[0] = 0;
                    pixel[1] = 0;
                    pixel[2] = 0;
                }
            }
        }
        //UpdateAuxiliaryData();
        Display();
    }

    void VisionShopApp::DoGBandSaliencyDetection(void)
    {
        cv::Mat saliencyImg = MagicDIP::SaliencyDetection::DoGBandSaliency(mImage);
        mImage.release();
        mImage = saliencyImg.clone();
        saliencyImg.release();
        UpdateAuxiliaryData();
        Display();
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
        mMouseMode = MM_View;
        cv::Mat segImg = MagicDIP::Segmentation::SegmentByGraphCut(mImage, mMarkImage);
        UpdateDisplayImage(segImg);
        segImg.release();
        Display();
    }

    void VisionShopApp::ImageClustering(int k)
    {
        int imgW = mImage.cols;
        int imgH = mImage.rows;
        int dim = 3;
        std::vector<double> inputData;
        for (int hid = 0; hid < imgH; hid++)
        {
            for (int wid = 0; wid < imgW; wid++)
            {
                unsigned char* pPixel = mImage.ptr(hid, wid);
                inputData.push_back(pPixel[0]);
                inputData.push_back(pPixel[1]);
                inputData.push_back(pPixel[2]);
            }
        }
        std::vector<int> clusterRes;
        MagicML::Clustering::OrchardBouman(inputData, dim, k, clusterRes);
        //MagicML::Clustering::KMeans(inputData, dim, k, clusterRes);
        std::vector<int> clusterCount(k, 0);
        std::vector<double> clusterColor(k * dim, 0);
        int pixelIndex = 0;
        for (int hid = 0; hid < imgH; hid++)
        {
            for (int wid = 0; wid < imgW; wid++)
            {
                unsigned char* pPixel = mImage.ptr(hid, wid);
                int cid = clusterRes.at(pixelIndex);
                clusterCount.at(cid)++;
                clusterColor.at(cid * dim + 0) += pPixel[0];
                clusterColor.at(cid * dim + 1) += pPixel[1];
                clusterColor.at(cid * dim + 2) += pPixel[2];
                pixelIndex++;
            }
        }
        for (int cid = 0; cid < k; cid++)
        {
            if (clusterCount.at(cid) > 0)
            {
                clusterColor.at(cid * dim + 0) /= clusterCount.at(cid);
                clusterColor.at(cid * dim + 1) /= clusterCount.at(cid);
                clusterColor.at(cid * dim + 2) /= clusterCount.at(cid);
            }
        }
        pixelIndex = 0;
        for (int hid = 0; hid < imgH; hid++)
        {
            for (int wid = 0; wid < imgW; wid++)
            {
                int cid = clusterRes.at(pixelIndex);
                unsigned char* pPixel = mImage.ptr(hid, wid);
                pPixel[0] = clusterColor.at(cid * dim + 0);
                pPixel[1] = clusterColor.at(cid * dim + 1);
                pPixel[2] = clusterColor.at(cid * dim + 2);
                pixelIndex++;
            }
        }
        UpdateAuxiliaryData();
        Display();
    }

    void VisionShopApp::Deformation(void)
    {
        if (mMouseMode == MM_Deformation)
        {
            mDeformationMarks.clear();
            UpdateDisplayImage(mDeformationMarks);
            Display();
            mMouseMode = MM_View;
        }
        else
        {
            mIsNewImage = true;
            UpdateAuxiliaryData();
            Display();
            mMouseMode = MM_Deformation;
        }
    }
}

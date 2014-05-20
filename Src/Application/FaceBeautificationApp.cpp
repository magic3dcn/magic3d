#include "FaceBeautificationApp.h"
#include "flann/flann.h"
#include "../DIP/Deformation.h"
#include "../Tool/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../DIP/Saliency.h"
#include <stdio.h>

namespace MagicApp
{
    FaceBeautificationApp::FaceBeautificationApp() :
        mFeaturePointSelected(false),
        mMouseMode(MM_View),
        mMaxFaceWidth(0),
        mMaxFaceHeight(0),
        mAutoAlignPoints()
    {
    }

    FaceBeautificationApp::~FaceBeautificationApp()
    {
    }

    bool FaceBeautificationApp::Enter(void)
    {
        InfoLog << "Enter FaceBeautificationApp" << std::endl;
        mUI.Setup(&mMaxFaceWidth, &mMaxFaceHeight);
        SetupScene();

        return true;
    }

    bool FaceBeautificationApp::Update(float timeElapsed)
    {
        return true;
    }

    bool FaceBeautificationApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();

        return true;
    }

    bool FaceBeautificationApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        if (mMouseMode == MM_Move_Origin_Feature)
        {
            if (arg.state.buttonDown(OIS::MB_Left))
            {
                if (mFeaturePointSelected)
                {
                    int hPos = arg.state.Y.abs - 50;
                    int wPos = arg.state.X.abs - 90;
                    int imgH, imgW;
                    mFace2D.GetImageSize(&imgW, &imgH);
                    if (wPos >= 0 && wPos < imgW && hPos >= 0 && hPos < imgH)
                    {
                        mFace2D.GetFps()->MoveTo(hPos, wPos);
                        std::vector<int> fpsList;
                        mFace2D.GetFps()->GetFPs(fpsList);
                        std::vector<int> dpsList;
                        mFace2D.GetFps()->GetDPs(dpsList);
                        UpdateLeftDisplayImage(&dpsList, &fpsList);
                        mUI.UpdateLeftImage(mLeftDisplayImage);
                    }
                }
            }
        }
        
        return true;
    }

    bool FaceBeautificationApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_Move_Origin_Feature)
        {
            if (id == OIS::MB_Left)
            {
                int hPos = arg.state.Y.abs - 50;
                int wPos = arg.state.X.abs - 90;
                mFeaturePointSelected = mFace2D.GetFps()->Select(hPos, wPos);
            }
            /*if (id == OIS::MB_Left)
            {
                int hPos = arg.state.Y.abs - 50;
                int wPos = arg.state.X.abs - 90;
                mMarkPoints.push_back(cv::Point2f(wPos, hPos));
                if (mMarkPoints.size() == 3)
                {
                    double eyeLeftX, eyeLeftY, eyeRightX, eyeRightY;
                    mFace2D.GetFps()->GetEyeCenter(eyeLeftX, eyeLeftY, eyeRightX, eyeRightY);
                    double mouseX, mouseY;
                    mFace2D.GetFps()->GetMouseCenter(mouseX, mouseY);
                    std::vector<cv::Point2f> cvTargetFps(3);
                    cvTargetFps.at(0) = cv::Point2f(eyeLeftX, eyeLeftY);
                    cvTargetFps.at(1) = cv::Point2f(eyeRightX, eyeRightY);
                    cvTargetFps.at(2) = cv::Point2f(mouseX, mouseY);
                    cv::Mat transMat = cv::estimateRigidTransform(cvTargetFps, mMarkPoints, false);
                    MagicMath::HomoMatrix3 fpsTransform;
                    fpsTransform.SetValue(0, 0, transMat.at<double>(0, 0));
                    fpsTransform.SetValue(0, 1, transMat.at<double>(0, 1));
                    fpsTransform.SetValue(0, 2, transMat.at<double>(0, 2));
                    fpsTransform.SetValue(1, 0, transMat.at<double>(1, 0));
                    fpsTransform.SetValue(1, 1, transMat.at<double>(1, 1));
                    fpsTransform.SetValue(1, 2, transMat.at<double>(1, 2));
                    std::vector<int> fpsList;
                    mFace2D.GetFps()->GetFPs(fpsList);
                    int fpsSize = fpsList.size() / 2;
                    for (int fpsId = 0; fpsId < fpsSize; fpsId++)
                    {
                        double resX, resY;
                        fpsTransform.TransformPoint(fpsList.at(fpsId * 2 + 1), fpsList.at(fpsId * 2), resX, resY);
                        fpsList.at(fpsId * 2 + 1) = resX;
                        fpsList.at(fpsId * 2) = resY;
                    }
                    UpdateLeftDisplayImage(NULL, &fpsList);
                    mUI.UpdateLeftImage(mLeftDisplayImage);
                    mMarkPoints.clear();
                    mMouseMode = MM_View;
                }
            }*/
            
        }
        
        return true;
    }

    bool FaceBeautificationApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_Move_Origin_Feature)
        {
            if (id == OIS::MB_Left)
            {
                if (mFeaturePointSelected)
                {
                    std::vector<int> fpsList;
                    mFace2D.GetFps()->GetFPs(fpsList);
                    UpdateLeftDisplayImage(NULL, &fpsList);
                    mUI.UpdateLeftImage(mLeftDisplayImage);
                    mFeaturePointSelected = false;
                }
            }
        }
        
        return true;
    }

    bool FaceBeautificationApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (mFeaturePointSelected)
        {
            int deltaH = 0;
            int deltaW = 0;
            if (arg.key == OIS::KC_LEFT)
            {
                deltaW = -1;
            }
            else if (arg.key == OIS::KC_RIGHT)
            {
                deltaW = 1;
            }
            else if (arg.key == OIS::KC_UP)
            {
                deltaH = -1;
            }
            else if (arg.key == OIS::KC_DOWN)
            {
                deltaH = 1;
            }
            mFace2D.GetFps()->MoveDelta(deltaH, deltaW);
            std::vector<int> fpsList;
            mFace2D.GetFps()->GetFPs(fpsList);
            std::vector<int> dpsList;
            mFace2D.GetFps()->GetDPs(dpsList);
            UpdateLeftDisplayImage(&dpsList, &fpsList);
            mUI.UpdateLeftImage(mLeftDisplayImage);
        }
        
        return true;
    }

    void FaceBeautificationApp::WindowResized( Ogre::RenderWindow* rw )
    {
        
    }

    void FaceBeautificationApp::SetupScene(void)
    {

    }

    void FaceBeautificationApp::ShutdownScene(void)
    {

    }

    void FaceBeautificationApp::MarkPointsToImage(cv::Mat& img, const std::vector<int>* markList, 
            unsigned char blue, unsigned char green, unsigned char red, int markWidth)
    {
        if (markList != NULL)
        {
            int imgW = img.cols;
            int imgH = img.rows;
            int markNum = markList->size() / 2;
            for (int mid = 0; mid < markNum; mid++)
            {
                int wPos = markList->at(2 * mid + 1);
                int hPos = markList->at(2 * mid);
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
                        unsigned char* pixel = img.ptr(hid, wid);
                        pixel[0] = blue;
                        pixel[1] = green;
                        pixel[2] = red;
                    }
                }
            }
        }
    }

    void FaceBeautificationApp::UpdateLeftDisplayImage(const std::vector<int>* dpsList, const std::vector<int>* fpsList)
    {
        mLeftDisplayImage.release();
        mLeftDisplayImage = mFace2D.GetImage().clone();
        MarkPointsToImage(mLeftDisplayImage, dpsList, 0, 255, 255, 1);
        MarkPointsToImage(mLeftDisplayImage, fpsList, 0, 0, 255, 1);
    }

    void FaceBeautificationApp::UpdateRightDisplayImage(const std::vector<int>* fpsList)
    {
        mRightDisplayImage.release();
        mRightDisplayImage = mFace2D.GetRefImage().clone();
        MarkPointsToImage(mRightDisplayImage, fpsList, 0, 0, 255, 1);
    }

    void FaceBeautificationApp::UpdateMidDisplayImage(const cv::Mat& img)
    {
        mMidDisplayImage.release();
        mMidDisplayImage = img.clone();
    }

    void FaceBeautificationApp::UpdateMidDisplayImage(const std::vector<int>& markList)
    {
        mMidDisplayImage.release();
        mMidDisplayImage.create(mMaxFaceHeight, mMaxFaceWidth, CV_8UC3);
        for (int hid = 0; hid < mMaxFaceHeight; hid++)
        {
            for (int wid = 0; wid < mMaxFaceWidth; wid++)
            {
                unsigned char* pixel = mMidDisplayImage.ptr(hid, wid);
                pixel[0] = 220;
                pixel[1] = 220;
                pixel[2] = 220;
            }
        }
        MarkPointsToImage(mMidDisplayImage, &markList, 0, 0, 255, 1);
    }

    bool FaceBeautificationApp::OpenImage(void)
    {
        std::string fileName;
        char filterName[] = "Image Files(*.jpg)\0*.jpg\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mFace2D.LoadImage(fileName);
            mFpsPath = fileName;
            std::string::size_type pos = mFpsPath.rfind(".");
            mFpsPath.replace(pos, 4, ".fp");
            if (!mFace2D.LoadFps(mFpsPath))
            {
                mFace2D.LoadFps("../../Media/FaceData/mean.fp");
                mFace2D.GetFps()->Save(mFpsPath);
            }
            //scale image to max size
            mFace2D.SetMaxImageSize(mMaxFaceWidth, mMaxFaceHeight);
            //update app state
            mMouseMode = MM_View;
            UpdateLeftDisplayImage(NULL, NULL);
            mUI.UpdateLeftImage(mLeftDisplayImage);

            return true;
        }
        else
        {
            return false;
        }
    }

    void FaceBeautificationApp::EditFeaturePoint(void)
    {
        if (mMouseMode != MM_Move_Origin_Feature)
        {
            mMouseMode = MM_Move_Origin_Feature;
            //Display feature point
            std::vector<int> fpsList;
            mFace2D.GetFps()->GetFPs(fpsList);
            UpdateLeftDisplayImage(NULL, &fpsList);
            mUI.UpdateLeftImage(mLeftDisplayImage);
        }
        else
        {
            mMouseMode = MM_View;
            UpdateLeftDisplayImage(NULL, NULL);
            mUI.UpdateLeftImage(mLeftDisplayImage);
            std::string fpsBakName = mFpsPath + ".bak";
            rename(mFpsPath.c_str(), fpsBakName.c_str());
            mFace2D.GetFps()->Save(mFpsPath);
        }
    }

    void FaceBeautificationApp::AutoAlignFeature()
    {
        if (mMouseMode != MM_Auto_Align_Feature)
        {
            mMouseMode = MM_Auto_Align_Feature;
            mAutoAlignPoints.clear();
        }
        else
        {
            mMouseMode = MM_View;
            mAutoAlignPoints.clear();
            UpdateLeftDisplayImage(NULL, NULL);
            mUI.UpdateLeftImage(mLeftDisplayImage);
        }
    }

    void FaceBeautificationApp::DeformImageFeature(void)
    {
        cv::Mat deformImg = mFace2D.DeformImageByFeature();
        UpdateMidDisplayImage(deformImg);
        mUI.UpdateMiddleImage(mMidDisplayImage);
    }

    void FaceBeautificationApp::DeformImageColor(void)
    {

    }

    bool FaceBeautificationApp::OpenReferenceImage(void)
    {
        std::string fileName;
        char filterName[] = "Image Files(*.jpg)\0*.jpg\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mFace2D.LoadRefImage(fileName);
            std::string fpsName = fileName;
            std::string::size_type pos = fpsName.rfind(".");
            fpsName.replace(pos, 4, ".fp");
            if (!mFace2D.LoadRefFps(fpsName))
            {
                return false;
            }
            //scale image to max size
            mFace2D.SetMaxRefImageSize(mMaxFaceWidth, mMaxFaceHeight);
            //update app state
            std::vector<int> refFps;
            mFace2D.GetRefFps()->GetFPs(refFps);
            UpdateRightDisplayImage(&refFps);
            mUI.UpdateRightImage(mRightDisplayImage);

            return true;
        }
        else
        {
            return false;
        }
    }

    bool FaceBeautificationApp::CalReferenceImage(void)
    {
        std::vector<int> imgIndex;
        ReadImgIndex(&imgIndex);
        mFace2D.CalRefFpsByProjectPca("../../Media/FaceData/g", imgIndex);

        //Deform projected fps to img
        std::vector<int> fps, refFps;
        mFace2D.GetFps()->GetFPs(fps);
        mFace2D.GetRefFps()->GetFPs(refFps);
        int fpsSize = fps.size() / 2;
        std::vector<cv::Point2f> cvFps(fpsSize);
        std::vector<cv::Point2f> cvRefFps(fpsSize);
        for (int fpsId = 0; fpsId < fpsSize; fpsId++)
        {
            cvFps.at(fpsId).x = fps.at(fpsId * 2 + 1);
            cvFps.at(fpsId).y = fps.at(fpsId * 2);
            cvRefFps.at(fpsId).x = refFps.at(fpsId * 2 + 1);
            cvRefFps.at(fpsId).y = refFps.at(fpsId * 2);
        }
        cv::Mat transMat = cv::estimateRigidTransform(cvRefFps, cvFps, false);
        MagicMath::HomoMatrix3 homoTransMat;
        homoTransMat.SetValue(0, 0, transMat.at<double>(0, 0));
        homoTransMat.SetValue(0, 1, transMat.at<double>(0, 1));
        homoTransMat.SetValue(0, 2, transMat.at<double>(0, 2));
        homoTransMat.SetValue(1, 0, transMat.at<double>(1, 0));
        homoTransMat.SetValue(1, 1, transMat.at<double>(1, 1));
        homoTransMat.SetValue(1, 2, transMat.at<double>(1, 2));
        for (int fpsId = 0; fpsId < fpsSize; fpsId++)
        {
            double xRes, yRes;
            homoTransMat.TransformPoint(cvRefFps.at(fpsId).x, cvRefFps.at(fpsId).y, xRes, yRes);
            refFps.at(fpsId * 2) = floor(yRes + 0.5);
            refFps.at(fpsId * 2 + 1) = floor(xRes + 0.5);
        }
        //update display
        mRightDisplayImage.release();
        mRightDisplayImage = mFace2D.GetImage().clone();
        MarkPointsToImage(mRightDisplayImage, &refFps, 0, 0, 255, 1);
        mUI.UpdateRightImage(mRightDisplayImage);

        return true;
    }

    void FaceBeautificationApp::DoFeaturePca(void)
    {
        std::vector<int> imgIndex;
        ReadImgIndex(&imgIndex);
        mFace2D.DoFeaturePca("../../Media/FaceData/g", imgIndex);
        std::vector<double> meanVec = mFace2D.GetFeaturePca()->GetMeanVector();
        std::vector<int> meanFps(meanVec.size());
        for (int i = 0; i < meanVec.size(); i++)
        {
            meanFps.at(i) = floor(meanVec.at(i) + 0.5);
        }
        UpdateMidDisplayImage(meanFps);
        mUI.UpdateMiddleImage(mMidDisplayImage);
    }

    void FaceBeautificationApp::DoColorPca(void)
    {

    }

    void FaceBeautificationApp::DoMixPca(void)
    {

    }

    void FaceBeautificationApp::ReadImgIndex(std::vector<int>* imgIndex)
    {
        std::ifstream fin("../../Media/FaceData/g.cfg");
        int imgCount;
        fin >> imgCount;
        imgIndex->clear();
        imgIndex->resize(imgCount);
        for (int imgId = 0; imgId < imgCount; imgId++)
        {
            fin >> imgIndex->at(imgId);
        }
    }

    void FaceBeautificationApp::CalMeanFace(void)
    {
        std::vector<int> imgIndex;
        ReadImgIndex(&imgIndex);
        mFace2D.CalMeanFace("../../Media/FaceData/g", imgIndex);
    }

    void FaceBeautificationApp::DeformFeatureToMeanFace(void)
    {
        std::vector<int> imgIndex;
        ReadImgIndex(&imgIndex);
        mFace2D.DeformFeatureToMeanFace("../../Media/FaceData/g", imgIndex);
    }

    void FaceBeautificationApp::DeformColorToMeanFace(void)
    {

    }

    void FaceBeautificationApp::DeformToMeanFace(void)
    {

    }

    //void FaceBeautificationApp::ObservePca(void)
    //{
    //    static int pcaId = -1;
    //    static int deltaId = 0;
    //    if (pcaId == -1)
    //    {
    //        std::vector<double> eigenVec = mPca.GetAvgVector();
    //        std::vector<int> markIndex(eigenVec.size());
    //        for (int mid = 0; mid < eigenVec.size(); mid++)
    //        {
    //            markIndex.at(mid) = eigenVec.at(mid);
    //        }
    //        UpdateRightDisplayImage(markIndex);
    //        pcaId++;
    //    }
    //    else
    //    {
    //        std::vector<double> meanVec = mPca.GetAvgVector();
    //        std::vector<double> eigenVec = mPca.GetEigenVector(pcaId);
    //        double eigenValue = mPca.GetEigenValue(pcaId);
    //        double maxCoef = 10 * sqrt(eigenValue);
    //        double minCoef = -maxCoef;
    //        double curCoef = minCoef + deltaId;
    //        if (curCoef > maxCoef)
    //        {
    //            deltaId = 0;
    //            pcaId++;
    //        }
    //        else
    //        {
    //            deltaId += 5;
    //        }
    //        std::vector<int> markIndex(eigenVec.size());
    //        for (int mid = 0; mid < eigenVec.size(); mid++)
    //        {
    //            markIndex.at(mid) = floor(meanVec.at(mid) + eigenVec.at(mid) * curCoef + 0.5);
    //        }
    //        UpdateRightDisplayImage(markIndex);
    //    }
    //    mUI.UpdateRightImage(mRightDisplayImage);
    //    //pcaId++;
    //}

    //void FaceBeautificationApp::ProjectRefFeatures(void)
    //{
    //    static bool isProjected = false;

    //    std::vector<int> fps;
    //    mRefFPs.GetFPs(fps);
    //    int pointCount = fps.size() / 2;
    //    std::vector<double> avgVec = mPca.GetAvgVector();
    //    std::vector<cv::Point2f> cvfps(pointCount);
    //    std::vector<cv::Point2f> cvavg(pointCount);
    //    for (int pointId = 0; pointId < pointCount; pointId++)
    //    {
    //        cvfps.at(pointId).x = fps.at(pointId * 2 + 1);
    //        cvfps.at(pointId).y = fps.at(pointId * 2);
    //        cvavg.at(pointId).x = avgVec.at(pointId * 2 + 1);
    //        cvavg.at(pointId).y = avgVec.at(pointId * 2);
    //    }
    //    cv::Mat transMat = cv::estimateRigidTransform(cvfps, cvavg, false);
    //    mRefFPTranform.Unit();
    //    mRefFPTranform.SetValue(0, 0, transMat.at<double>(0, 0));
    //    mRefFPTranform.SetValue(0, 1, transMat.at<double>(0, 1));
    //    mRefFPTranform.SetValue(0, 2, transMat.at<double>(0, 2));
    //    mRefFPTranform.SetValue(1, 0, transMat.at<double>(1, 0));
    //    mRefFPTranform.SetValue(1, 1, transMat.at<double>(1, 1));
    //    mRefFPTranform.SetValue(1, 2, transMat.at<double>(1, 2));
    //    //
    //    std::vector<double> unifiedVec(pointCount * 2);
    //    for (int pointId = 0; pointId < pointCount; pointId++)
    //    {
    //        double xRes, yRes;
    //        mRefFPTranform.TransformPoint(cvfps.at(pointId).x, cvfps.at(pointId).y, xRes, yRes);
    //        unifiedVec.at(pointId * 2) = yRes;
    //        unifiedVec.at(pointId * 2 + 1) = xRes;
    //    }
    //    if (isProjected)
    //    {
    //        std::vector<double> projectVec = mPca.Project(unifiedVec);
    //        std::vector<int> markIndex(pointCount * 2);
    //        MagicMath::HomoMatrix3 revMat = mRefFPTranform.ReverseRigidTransform();
    //        for (int pointId = 0; pointId < pointCount; pointId++)
    //        {
    //            double xRes, yRes;
    //            revMat.TransformPoint(projectVec.at(pointId * 2 + 1), projectVec.at(pointId * 2), xRes, yRes);
    //            markIndex.at(pointId * 2 + 1) = floor(xRes + 0.5);
    //            markIndex.at(pointId * 2) = floor(yRes + 0.5);
    //        }
    //        UpdateMidDisplayImage(&markIndex);
    //    }
    //    else
    //    {
    //        std::vector<int> markIndex(pointCount * 2);
    //        MagicMath::HomoMatrix3 revMat = mRefFPTranform.ReverseRigidTransform();
    //        for (int pointId = 0; pointId < pointCount; pointId++)
    //        {
    //            double xRes, yRes;
    //            revMat.TransformPoint(unifiedVec.at(pointId * 2 + 1), unifiedVec.at(pointId * 2), xRes, yRes);
    //            markIndex.at(pointId * 2 + 1) = floor(xRes + 0.5);
    //            markIndex.at(pointId * 2) = floor(yRes + 0.5);
    //        }
    //        UpdateMidDisplayImage(&markIndex);
    //    }
    //    
    //    mUI.UpdateMiddleImage(mMidDisplayImage);

    //    isProjected = !isProjected;
    //}

}
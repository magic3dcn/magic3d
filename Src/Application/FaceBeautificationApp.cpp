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
        mMaxFaceHeight(0)
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

    /*void FaceBeautificationApp::UpdateMidDisplayImage(const std::vector<int>* markIndex)
    {
        mMidDisplayImage.release();
        mMidDisplayImage = mRefImage.clone();
        if (markIndex != NULL)
        {
            int imgW = mRefImage.cols;
            int imgH = mRefImage.rows;
            int markNum = markIndex->size() / 2;
            int markWidth = 1;
            for (int mid = 0; mid < markNum; mid++)
            {
                int wPos = markIndex->at(2 * mid + 1);
                int hPos = markIndex->at(2 * mid);
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
                        unsigned char* pixel = mMidDisplayImage.ptr(hid, wid);
                        pixel[0] = 0;
                        pixel[1] = 255;
                        pixel[2] = 0;
                    }
                }
            }
        }
    }

    void FaceBeautificationApp::UpdateRightDisplayImage(const cv::Mat& img, const std::vector<int>* leftMarkIndex, 
        const std::vector<int>* midMarkIndex, const MagicMath::HomoMatrix3& midTransform)
    {
        mRightDisplayImage.release();
        mRightDisplayImage = img.clone();
        if (midMarkIndex != NULL)
        {
            int imgW = mRightDisplayImage.cols;
            int imgH = mRightDisplayImage.rows;
            int markNum = midMarkIndex->size() / 2;
            int markWidth = 1;
            for (int mid = 0; mid < markNum; mid++)
            {
                int wPosRef = midMarkIndex->at(2 * mid + 1);
                int hPosRef = midMarkIndex->at(2 * mid);
                double wPosRes, hPosRes;
                midTransform.TransformPoint(wPosRef, hPosRef, wPosRes, hPosRes);
                int wPos = wPosRes;
                int hPos = hPosRes;
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
                        unsigned char* pixel = mRightDisplayImage.ptr(hid, wid);
                        pixel[0] = 0;
                        pixel[1] = 255;
                        pixel[2] = 0;
                    }
                }
            }
        }
        if (leftMarkIndex != NULL)
        {
            int imgW = mRightDisplayImage.cols;
            int imgH = mRightDisplayImage.rows;
            int markNum = leftMarkIndex->size() / 2;
            int markWidth = 1;
            for (int mid = 0; mid < markNum; mid++)
            {
                int wPos = leftMarkIndex->at(2 * mid + 1);
                int hPos = leftMarkIndex->at(2 * mid);
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
                        unsigned char* pixel = mRightDisplayImage.ptr(hid, wid);
                        pixel[0] = 0;
                        pixel[1] = 0;
                        pixel[2] = 255;
                    }
                }
            }
        }
    }

    void FaceBeautificationApp::UpdateRightDisplayImage(const std::vector<FaceFeaturePoint*>& faceFeatureList, 
        const std::vector<MagicMath::HomoMatrix3*>& transformList)
    {
        int winW = 300;
        int winH = 375;
        mRightDisplayImage.release();
        cv::Size imgSize(winW, winH);
        mRightDisplayImage.create(imgSize, CV_8UC3);
        int faceNum = faceFeatureList.size();
        for (int faceId = 0; faceId < faceNum; faceId++)
        {
            std::vector<int> fps;
            faceFeatureList.at(faceId)->GetFPs(fps);
            int markNum = fps.size() / 2;
            for (int markId = 0; markId < markNum; markId++)
            {
                int wPosRef = fps.at(2 * markId + 1);
                int hPosRef = fps.at(2 * markId);
                double wPosRes, hPosRes;
                transformList.at(faceId)->TransformPoint(wPosRef, hPosRef, wPosRes, hPosRes);
                int wPos = wPosRes;
                int hPos = hPosRes;
                unsigned char* pixel = mRightDisplayImage.ptr(hPos, wPos);
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 255;
            }
        }
    }

    void FaceBeautificationApp::UpdateRightDisplayImage(const std::vector<int>& markIndex)
    {
        int winW = 300;
        int winH = 375;
        mRightDisplayImage.release();
        cv::Size imgSize(winW, winH);
        mRightDisplayImage.create(imgSize, CV_8UC3);
        for (int hid = 0; hid < winH; hid++)
        {
            for (int wid = 0; wid < winW; wid++)
            {
                unsigned char* pixel = mRightDisplayImage.ptr(hid, wid);
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
            }
        }
        int markNum = markIndex.size() / 2;
        for (int markId = 0; markId < markNum; markId++)
        {
            int wPos = markIndex.at(2 * markId + 1);
            int hPos = markIndex.at(2 * markId);
            unsigned char* pixel = mRightDisplayImage.ptr(hPos, wPos);
            pixel[0] = 0;
            pixel[1] = 0;
            pixel[2] = 255;
        }
    }*/

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
        return true;
    }

    void FaceBeautificationApp::DoFeaturePca(void)
    {
        mFace2D.DoFeaturePca("../../Media/FaceData/g", 41);
    }

    void FaceBeautificationApp::DoColorPca(void)
    {

    }

    void FaceBeautificationApp::DoMixPca(void)
    {

    }

    void FaceBeautificationApp::CalMeanFace(void)
    {

    }

    void FaceBeautificationApp::DeformFeatureToMeanFace(void)
    {

    }

    void FaceBeautificationApp::DeformColorToMeanFace(void)
    {

    }

    void FaceBeautificationApp::DeformToMeanFace(void)
    {

    }

    //void FaceBeautificationApp::AlignToMean(void)
    //{
    //    //Get mean feature
    //    int browNum, eyeNum, noseNum, mouseNum, borderNum;
    //    mOriginFPs.GetParameter(browNum, eyeNum, noseNum, mouseNum, borderNum);
    //    std::vector<double> meanFeaturePosList_d = mPca.GetAvgVector();
    //    std::vector<int> meanFeaturePosList(meanFeaturePosList_d.size());
    //    for (int vid = 0; vid < meanFeaturePosList_d.size(); vid++)
    //    {
    //        meanFeaturePosList.at(vid) = floor(meanFeaturePosList_d.at(vid) + 0.5);
    //    }
    //    FaceFeaturePoint meanFPs;
    //    meanFPs.Load(browNum, eyeNum, noseNum, mouseNum, borderNum, meanFeaturePosList);
    //    
    //    //Transform to mean feature
    //    std::vector<int> originfps;
    //    mOriginFPs.GetFPs(originfps);
    //    int featureSize = originfps.size() / 2;
    //    std::vector<cv::Point2f> cvOriginFeatures(featureSize);
    //    std::vector<cv::Point2f> cvMeanFeatures(featureSize);
    //    for (int fid = 0; fid < featureSize; fid++)
    //    {
    //        cvOriginFeatures.at(fid).x = originfps.at(fid * 2 + 1);
    //        cvOriginFeatures.at(fid).y = originfps.at(fid * 2);
    //        cvMeanFeatures.at(fid).x = meanFeaturePosList_d.at(fid * 2 + 1);
    //        cvMeanFeatures.at(fid).y = meanFeaturePosList_d.at(fid * 2);
    //    }
    //    cv::Mat transMat = cv::estimateRigidTransform(cvOriginFeatures, cvMeanFeatures, false);
    //    MagicMath::HomoMatrix3 homoTransMat;
    //    homoTransMat.SetValue(0, 0, transMat.at<double>(0, 0));
    //    homoTransMat.SetValue(0, 1, transMat.at<double>(0, 1));
    //    homoTransMat.SetValue(0, 2, transMat.at<double>(0, 2));
    //    homoTransMat.SetValue(1, 0, transMat.at<double>(1, 0));
    //    homoTransMat.SetValue(1, 1, transMat.at<double>(1, 1));
    //    homoTransMat.SetValue(1, 2, transMat.at<double>(1, 2));
    //    homoTransMat.Print();
    //    std::vector<int> originTransFps(featureSize * 2);
    //    for (int fid = 0; fid < featureSize; fid++)
    //    {
    //        double xRes, yRes;
    //        homoTransMat.TransformPoint(originfps.at(fid * 2 + 1), originfps.at(fid * 2), xRes, yRes);
    //        originTransFps.at(fid * 2) = floor(yRes + 0.5);
    //        originTransFps.at(fid * 2 + 1) = floor(xRes + 0.5);
    //    }
    //    //Rigid transform image
    //    cv::Mat leftImgCopy = mImage.clone();
    //    mImage.release();
    //    cv::warpAffine(leftImgCopy, mImage, transMat, mImage.size());
    //    //UpdateLeftDisplayImage(&originTransFps, &meanFeaturePosList);
    //    //mUI.UpdateLeftImage(mLeftDisplayImage);

    //    //Deform to mean feature
    //    FaceFeaturePoint originTransFeature;
    //    originTransFeature.Load(browNum, eyeNum, noseNum, mouseNum, borderNum, originTransFps);
    //    std::vector<int> originTransDps, meanDps;
    //    originTransFeature.GetDPs(originTransDps);
    //    meanFPs.GetDPs(meanDps);
    //    for (int fid = 0; fid < originTransDps.size() / 2; fid++)
    //    {
    //        int temp = originTransDps.at(fid * 2);
    //        originTransDps.at(fid * 2) = originTransDps.at(fid * 2 + 1);
    //        originTransDps.at(fid * 2 + 1) = temp;

    //        temp = meanDps.at(fid * 2);
    //        meanDps.at(fid * 2) = meanDps.at(fid * 2 + 1);
    //        meanDps.at(fid * 2 + 1) = temp;
    //    }
    //    cv::Mat deformImg = MagicDIP::Deformation::DeformByMovingLeastSquares(mImage, originTransDps, meanDps);
    //    mImage.release();
    //    mImage = deformImg.clone();
    //    
    //    //Update display
    //    UpdateLeftDisplayImage(&meanFeaturePosList, NULL);
    //    mUI.UpdateLeftImage(mLeftDisplayImage);
    //}

    //void FaceBeautificationApp::AlignAllToMean(void)
    //{
    //    int picCount = 41;
    //    for (int picId = 0; picId < picCount; picId++)
    //    {
    //        //Load file
    //        std::stringstream ss;
    //        ss << "../../Media/FaceData/g" << picId << ".jpg";
    //        std::string picName;
    //        ss >> picName;
    //        ss.clear();
    //        ss << "../../Media/FaceData/f" << picId << ".fp";
    //        std::string fpsName;
    //        ss >> fpsName;
    //        ss.clear();
    //        mImage.release();
    //        mImage = cv::imread(picName);
    //        cv::Mat resizedImg = ResizeInputImageToCanvas(mImage);
    //        mImage.release();
    //        mImage = resizedImg.clone();
    //        mOriginFPs.Load(fpsName);
    //        
    //        //Transform
    //        int browNum, eyeNum, noseNum, mouseNum, borderNum;
    //        mOriginFPs.GetParameter(browNum, eyeNum, noseNum, mouseNum, borderNum);
    //        std::vector<double> meanFeaturePosList_d = mPca.GetAvgVector();
    //        std::vector<int> meanFeaturePosList(meanFeaturePosList_d.size());
    //        for (int vid = 0; vid < meanFeaturePosList_d.size(); vid++)
    //        {
    //            meanFeaturePosList.at(vid) = floor(meanFeaturePosList_d.at(vid) + 0.5);
    //        }
    //        FaceFeaturePoint meanFPs;
    //        meanFPs.Load(browNum, eyeNum, noseNum, mouseNum, borderNum, meanFeaturePosList);
    //        //Transform to mean feature
    //        std::vector<int> originfps;
    //        mOriginFPs.GetFPs(originfps);
    //        int featureSize = originfps.size() / 2;
    //        std::vector<cv::Point2f> cvOriginFeatures(featureSize);
    //        std::vector<cv::Point2f> cvMeanFeatures(featureSize);
    //        for (int fid = 0; fid < featureSize; fid++)
    //        {
    //            cvOriginFeatures.at(fid).x = originfps.at(fid * 2 + 1);
    //            cvOriginFeatures.at(fid).y = originfps.at(fid * 2);
    //            cvMeanFeatures.at(fid).x = meanFeaturePosList_d.at(fid * 2 + 1);
    //            cvMeanFeatures.at(fid).y = meanFeaturePosList_d.at(fid * 2);
    //        }
    //        cv::Mat transMat = cv::estimateRigidTransform(cvOriginFeatures, cvMeanFeatures, false);
    //        MagicMath::HomoMatrix3 homoTransMat;
    //        homoTransMat.SetValue(0, 0, transMat.at<double>(0, 0));
    //        homoTransMat.SetValue(0, 1, transMat.at<double>(0, 1));
    //        homoTransMat.SetValue(0, 2, transMat.at<double>(0, 2));
    //        homoTransMat.SetValue(1, 0, transMat.at<double>(1, 0));
    //        homoTransMat.SetValue(1, 1, transMat.at<double>(1, 1));
    //        homoTransMat.SetValue(1, 2, transMat.at<double>(1, 2));
    //        homoTransMat.Print();
    //        std::vector<int> originTransFps(featureSize * 2);
    //        for (int fid = 0; fid < featureSize; fid++)
    //        {
    //            double xRes, yRes;
    //            homoTransMat.TransformPoint(originfps.at(fid * 2 + 1), originfps.at(fid * 2), xRes, yRes);
    //            originTransFps.at(fid * 2) = floor(yRes + 0.5);
    //            originTransFps.at(fid * 2 + 1) = floor(xRes + 0.5);
    //        }
    //        //Rigid transform image
    //        cv::Mat leftImgCopy = mImage.clone();
    //        mImage.release();
    //        cv::warpAffine(leftImgCopy, mImage, transMat, mImage.size());
    //        //Deform to mean feature
    //        FaceFeaturePoint originTransFeature;
    //        originTransFeature.Load(browNum, eyeNum, noseNum, mouseNum, borderNum, originTransFps);
    //        std::vector<int> originTransDps, meanDps;
    //        originTransFeature.GetDPs(originTransDps);
    //        meanFPs.GetDPs(meanDps);
    //        for (int fid = 0; fid < originTransDps.size() / 2; fid++)
    //        {
    //            int temp = originTransDps.at(fid * 2);
    //            originTransDps.at(fid * 2) = originTransDps.at(fid * 2 + 1);
    //            originTransDps.at(fid * 2 + 1) = temp;

    //            temp = meanDps.at(fid * 2);
    //            meanDps.at(fid * 2) = meanDps.at(fid * 2 + 1);
    //            meanDps.at(fid * 2 + 1) = temp;
    //        }
    //        cv::Mat deformImg = MagicDIP::Deformation::DeformByMovingLeastSquares(mImage, originTransDps, meanDps);
    //        mImage.release();
    //        //Write image
    //        ss << "../../Media/FaceData/tg" << picId << ".jpg";
    //        std::string transImgName;
    //        ss >> transImgName;
    //        cv::imwrite(transImgName, deformImg);
    //    }
    //}

    //void FaceBeautificationApp::CalMeanFace(void)
    //{
    //    int imgW = 300;
    //    int imgH = 375;
    //    std::vector<int> meanImgBlue(imgW * imgH, 0);
    //    std::vector<int> meanImgGreen(imgW * imgH, 0);
    //    std::vector<int> meanImgRed(imgW * imgH, 0);
    //    int picCount = 40;
    //    for (int picId = 1; picId <= picCount; picId++)
    //    {
    //        //Load file
    //        std::stringstream ss;
    //        ss << "../../Media/FaceData/tg" << picId << ".jpg";
    //        std::string picName;
    //        ss >> picName;
    //        mImage.release();
    //        mImage = cv::imread(picName);
    //        cv::Mat resizedImg = ResizeInputImageToCanvas(mImage);
    //        mImage.release();
    //        mImage = resizedImg.clone();
    //        for (int hid = 0; hid < imgH; hid++)
    //        {
    //            int baseIndex = hid * imgW;
    //            for (int wid = 0; wid < imgW; wid++)
    //            {
    //                unsigned char* pixel = mImage.ptr(hid, wid);
    //                meanImgBlue.at(baseIndex + wid) += pixel[0];
    //                meanImgGreen.at(baseIndex + wid) += pixel[1];
    //                meanImgRed.at(baseIndex + wid) += pixel[2];
    //            }
    //        }
    //    }
    //    cv::Size imgSize(imgW, imgH);
    //    cv::Mat meanImg(imgSize, CV_8UC3);
    //    for (int hid = 0; hid < imgH; hid++)
    //    {
    //        int baseIndex = hid * imgW;
    //        for (int wid = 0; wid < imgW; wid++)
    //        {
    //            unsigned char* pixel = meanImg.ptr(hid, wid);
    //            pixel[0] = float(meanImgBlue.at(baseIndex + wid)) / picCount;
    //            pixel[1] = float(meanImgGreen.at(baseIndex + wid)) / picCount;
    //            pixel[2] = float(meanImgRed.at(baseIndex + wid)) / picCount;
    //        }
    //    }
    //    std::stringstream ss;
    //    ss << "../../Media/FaceData/mg.jpg";
    //    std::string meanName;
    //    ss >> meanName;
    //    cv::imwrite(meanName, meanImg);
    //}

    //void FaceBeautificationApp::DeformMeanToIndividual()
    //{
    //    mImage.release();
    //    std::string picName = "../../Media/FaceData/mg.jpg";
    //    mImage = cv::imread(picName);
    //    cv::Mat resizedImg = ResizeInputImageToCanvas(mImage);
    //    mImage.release();
    //    mImage = resizedImg.clone();
    //    int picCount = 41;
    //    for (int picId = 0; picId < picCount; picId++)
    //    {
    //        //Load file
    //        std::stringstream ss;
    //        ss << "../../Media/FaceData/f" << picId << ".fp";
    //        std::string fpsName;
    //        ss >> fpsName;
    //        ss.clear();
    //        mOriginFPs.Load(fpsName);

    //        //Transform
    //        int browNum, eyeNum, noseNum, mouseNum, borderNum;
    //        mOriginFPs.GetParameter(browNum, eyeNum, noseNum, mouseNum, borderNum);
    //        std::vector<double> meanFeaturePosList_d = mPca.GetAvgVector();
    //        std::vector<int> meanFeaturePosList(meanFeaturePosList_d.size());
    //        for (int vid = 0; vid < meanFeaturePosList_d.size(); vid++)
    //        {
    //            meanFeaturePosList.at(vid) = floor(meanFeaturePosList_d.at(vid) + 0.5);
    //        }
    //        FaceFeaturePoint meanFPs;
    //        meanFPs.Load(browNum, eyeNum, noseNum, mouseNum, borderNum, meanFeaturePosList);
    //        //Transform mean feature
    //        std::vector<int> originfps;
    //        mOriginFPs.GetFPs(originfps);
    //        int featureSize = originfps.size() / 2;
    //        std::vector<cv::Point2f> cvOriginFeatures(featureSize);
    //        std::vector<cv::Point2f> cvMeanFeatures(featureSize);
    //        for (int fid = 0; fid < featureSize; fid++)
    //        {
    //            cvOriginFeatures.at(fid).x = originfps.at(fid * 2 + 1);
    //            cvOriginFeatures.at(fid).y = originfps.at(fid * 2);
    //            cvMeanFeatures.at(fid).x = meanFeaturePosList_d.at(fid * 2 + 1);
    //            cvMeanFeatures.at(fid).y = meanFeaturePosList_d.at(fid * 2);
    //        }
    //        cv::Mat transMat = cv::estimateRigidTransform(cvMeanFeatures, cvOriginFeatures, false);
    //        MagicMath::HomoMatrix3 homoTransMat;
    //        homoTransMat.SetValue(0, 0, transMat.at<double>(0, 0));
    //        homoTransMat.SetValue(0, 1, transMat.at<double>(0, 1));
    //        homoTransMat.SetValue(0, 2, transMat.at<double>(0, 2));
    //        homoTransMat.SetValue(1, 0, transMat.at<double>(1, 0));
    //        homoTransMat.SetValue(1, 1, transMat.at<double>(1, 1));
    //        homoTransMat.SetValue(1, 2, transMat.at<double>(1, 2));
    //        homoTransMat.Print();
    //        std::vector<int> meanTransFps(featureSize * 2);
    //        for (int fid = 0; fid < featureSize; fid++)
    //        {
    //            double xRes, yRes;
    //            homoTransMat.TransformPoint(meanFeaturePosList_d.at(fid * 2 + 1), meanFeaturePosList_d.at(fid * 2), xRes, yRes);
    //            meanTransFps.at(fid * 2) = floor(yRes + 0.5);
    //            meanTransFps.at(fid * 2 + 1) = floor(xRes + 0.5);
    //        }
    //        //Rigid transform image
    //        cv::Size imgSize(mImage.cols, mImage.rows);
    //        cv::Mat rigidTransImg(imgSize, CV_8UC3);
    //        cv::warpAffine(mImage, rigidTransImg, transMat, rigidTransImg.size());
    //        //Deform mean feature
    //        FaceFeaturePoint meanTransFeature;
    //        meanTransFeature.Load(browNum, eyeNum, noseNum, mouseNum, borderNum, meanTransFps);
    //        std::vector<int> originDps, meanTransDps;
    //        meanTransFeature.GetDPs(meanTransDps);
    //        mOriginFPs.GetDPs(originDps);
    //        for (int fid = 0; fid < meanTransDps.size() / 2; fid++)
    //        {
    //            int temp = meanTransDps.at(fid * 2);
    //            meanTransDps.at(fid * 2) = meanTransDps.at(fid * 2 + 1);
    //            meanTransDps.at(fid * 2 + 1) = temp;

    //            temp = originDps.at(fid * 2);
    //            originDps.at(fid * 2) = originDps.at(fid * 2 + 1);
    //            originDps.at(fid * 2 + 1) = temp;
    //        }
    //        cv::Mat deformImg = MagicDIP::Deformation::DeformByMovingLeastSquares(mImage, meanTransDps, originDps);
    //        //Write image
    //        ss << "../../Media/FaceData/mtg" << picId << ".jpg";
    //        std::string transImgName;
    //        ss >> transImgName;
    //        cv::imwrite(transImgName, deformImg);
    //    }
    //}


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

    /*cv::Mat FaceBeautificationApp::ResizeInputImageToCanvas(const cv::Mat& img) const
    {
        int winW = 300;
        int winH = 375;
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
    }*/
}
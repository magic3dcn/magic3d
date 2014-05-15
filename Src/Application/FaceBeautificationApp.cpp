#include "FaceBeautificationApp.h"
#include "flann/flann.h"
#include "../DIP/Deformation.h"
#include "../Tool/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../DIP/Saliency.h"

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
        /*if (mMouseMode == MM_Move_Origin_Feature)
        {
            if (arg.state.buttonDown(OIS::MB_Left))
            {
                if (mFeaturePointSelected)
                {
                    int hPos = arg.state.Y.abs - 50;
                    int wPos = arg.state.X.abs - 90;
                    int imgH = mImage.rows;
                    int imgW = mImage.cols;
                    if (wPos >= 0 && wPos < imgW && hPos >= 0 && hPos < imgH)
                    {
                        mOriginFPs.MoveTo(hPos, wPos);
                        std::vector<int> markIndex;
                        mOriginFPs.GetDPs(markIndex);
                        std::vector<int> featureIndex;
                        mOriginFPs.GetFPs(featureIndex);
                        UpdateLeftDisplayImage(&markIndex, &featureIndex);
                        mUI.UpdateLeftImage(mLeftDisplayImage);
                    }
                }
            }
        }*/
        
        return true;
    }

    bool FaceBeautificationApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        /*if (mMouseMode == MM_Move_Origin_Feature)
        {
            if (id == OIS::MB_Left)
            {
                int hPos = arg.state.Y.abs - 50;
                int wPos = arg.state.X.abs - 90;
                int tol = 5;
                mFeaturePointSelected = mOriginFPs.Select(hPos, wPos);
            }
        }*/
        
        return true;
    }

    bool FaceBeautificationApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        /*if (mMouseMode == MM_Move_Origin_Feature)
        {
            if (id == OIS::MB_Left)
            {
                if (mFeaturePointSelected)
                {
                    mFeaturePointSelected = false;
                }
            }
        }*/
        
        return true;
    }

    bool FaceBeautificationApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        /*if (mFeaturePointSelected)
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
            mOriginFPs.MoveDelta(deltaH, deltaW);
            std::vector<int> markIndex;
            mOriginFPs.GetDPs(markIndex);
            std::vector<int> featureIndex;
            mOriginFPs.GetFPs(featureIndex);
            UpdateLeftDisplayImage(&markIndex, &featureIndex);
            mUI.UpdateLeftImage(mLeftDisplayImage);
        }*/
        
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
        mLeftDisplayImage = mFace2D.GetImage();
        MarkPointsToImage(mLeftDisplayImage, dpsList, 0, 0, 255, 1);
        MarkPointsToImage(mLeftDisplayImage, fpsList, 0, 255, 255, 1);
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
        char filterName[] = "Image Files(*.*)\0*.*\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mFace2D.LoadImage(fileName);
            std::string fpsName = fileName;
            std::string::size_type pos = fpsName.rfind(".");
            fpsName.replace(pos, 4, ".fp");
            if (!mFace2D.LoadFps(fpsName))
            {
                mFace2D.LoadFps("../../Media/FaceData/mean.fp");
            }
            //scale image to max size
            mFace2D.SetMaxImageSize(mMaxFaceWidth, mMaxFaceHeight);
            //update app state
            mMouseMode = MM_View;
            mLeftDisplayImage.release();
            mLeftDisplayImage = mFace2D.GetImage();
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
        }
    }

    void FaceBeautificationApp::DeformImageFeature(void)
    {

    }

    void FaceBeautificationApp::DeformImageColor(void)
    {

    }

    bool FaceBeautificationApp::OpenReferenceImage(void)
    {
        return true;
    }

    bool FaceBeautificationApp::CalReferenceImage(void)
    {
        return true;
    }

    void FaceBeautificationApp::DoFeaturePca(void)
    {

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

    

    /*void FaceBeautificationApp::LoadFeaturePoint()
    {
        mOriginFPs.Load();
        std::vector<int> markIndex;
        mOriginFPs.GetDPs(markIndex);
        UpdateLeftDisplayImage(&markIndex, NULL);
        mUI.UpdateLeftImage(mLeftDisplayImage);

        std::vector<int> refMarkIndex;
        mRefFPs.GetDPs(refMarkIndex);
        UpdateRightDisplayImage(mImage, &markIndex, &refMarkIndex, mRefFPTranform);
        mUI.UpdateRightImage(mRightDisplayImage);
    }*/

    /*void FaceBeautificationApp::EditOriginFeaturePoint()
    {
        mMouseMode = MM_Move_Origin_Feature;
        std::vector<int> markIndex;
        mOriginFPs.GetDPs(markIndex);
        std::vector<int> featureIndex;
        mOriginFPs.GetFPs(featureIndex);
        UpdateLeftDisplayImage(&markIndex, &featureIndex);
        mUI.UpdateLeftImage(mLeftDisplayImage);
    }*/

    //void FaceBeautificationApp::AutoMoveOriginFeaturePoint(void)
    //{
    //    //Calculate gradient image to get feature set
    //    cv::Mat gradImg = MagicDIP::SaliencyDetection::GradientSaliency(mImage);
    //    int imgH = mImage.rows;
    //    int imgW = mImage.cols;
    //    std::vector<int> gradPoints;
    //    for (int hid = 0; hid < imgH; hid++)
    //    {
    //        for (int wid = 0; wid < imgW; wid++)
    //        {
    //            unsigned char* gradPixel = gradImg.ptr(hid, wid);
    //            unsigned char* imgPixel = mImage.ptr(hid, wid);
    //            if (gradPixel[0] > 50)
    //            {
    //                imgPixel[0] = 255;
    //                imgPixel[1] = 255;
    //                imgPixel[2] = 255;
    //                gradPoints.push_back(hid);
    //                gradPoints.push_back(wid);
    //            }
    //            else
    //            {
    //                imgPixel[0] = 0;
    //                imgPixel[1] = 0;
    //                imgPixel[2] = 0;
    //            }
    //        }
    //    }
    //    //UpdateLeftDisplayImage(NULL, NULL);
    //    

    //    //using flann to find the nearest correspondance for every fps
    //    int dim = 2;
    //    int refNum = gradPoints.size() / 2;
    //    float* dataSet = new float[refNum * dim];
    //    for (int dataId = 0; dataId < refNum; dataId++)
    //    {
    //        dataSet[dim * dataId] = gradPoints.at(dim * dataId);
    //        dataSet[dim * dataId + 1] = gradPoints.at(dim * dataId + 1);
    //    }
    //    std::vector<int> fps;
    //    mOriginFPs.GetFPs(fps);
    //    int searchNum = fps.size() / 2;
    //    float* searchSet = new float[searchNum * dim];
    //    for (int dataId = 0; dataId < searchNum; dataId++)
    //    {
    //        searchSet[dataId * dim] = fps.at(dataId * dim);
    //        searchSet[dataId * dim + 1] = fps.at(dataId * dim + 1);
    //    }
    //    int nn = 1;
    //    int* pIndex = new int[searchNum * nn];
    //    float* pDist = new float[searchNum * nn];
    //    FLANNParameters searchPara;
    //    searchPara = DEFAULT_FLANN_PARAMETERS;
    //    searchPara.algorithm = FLANN_INDEX_KDTREE;
    //    searchPara.trees = 8;
    //    searchPara.log_level = FLANN_LOG_INFO;
    //    searchPara.checks = 64;
    //    float speedup;
    //    flann_index_t indexId = flann_build_index(dataSet, refNum, dim, &speedup, &searchPara);
    //    flann_find_nearest_neighbors_index(indexId, searchSet, searchNum, pIndex, pDist, nn, &searchPara);
    //    
    //    //align
    //    std::vector<int> reffps(searchNum * 2);
    //    for (int sid = 0; sid < searchNum; sid++)
    //    {
    //        reffps.at(sid * 2) = gradPoints.at(pIndex[sid] * 2);
    //        reffps.at(sid * 2 + 1) = gradPoints.at(pIndex[sid] * 2 + 1);
    //    }

    //    //update display
    //    UpdateLeftDisplayImage(&fps, &reffps);
    //    mUI.UpdateLeftImage(mLeftDisplayImage);
    //}

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

    //void FaceBeautificationApp::SaveFeaturePoint()
    //{
    //    mOriginFPs.Save();
    //}

    //bool FaceBeautificationApp::OpenRefImage(void)
    //{
    //    std::string fileName;
    //    char filterName[] = "Image Files(*.*)\0*.*\0";
    //    if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
    //    {
    //        mRefImage.release();
    //        mRefImage = cv::imread(fileName);
    //        if (mRefImage.data != NULL)
    //        {
    //            mRefImage = ResizeInputImageToCanvas(mRefImage);
    //            std::vector<int> markIndex;
    //            mRefFPs.GetDPs(markIndex);
    //            UpdateMidDisplayImage(&markIndex);
    //            mUI.UpdateMiddleImage(mMidDisplayImage);

    //            return true;
    //        }
    //    }
    //    return false;
    //}

    //void FaceBeautificationApp::LoadRefFeaturePoint(void)
    //{
    //    mRefFPs.Load();
    //    std::vector<int> refMarkIndex;
    //    mRefFPs.GetDPs(refMarkIndex);
    //    UpdateMidDisplayImage(&refMarkIndex);
    //    mUI.UpdateMiddleImage(mMidDisplayImage);

    //    std::vector<int> originMarkIndex;
    //    mOriginFPs.GetDPs(originMarkIndex);
    //    UpdateRightDisplayImage(mImage, &originMarkIndex, &refMarkIndex, mRefFPTranform);
    //    mUI.UpdateRightImage(mRightDisplayImage);
    //}

    //void FaceBeautificationApp::AlignFeature(void)
    //{
    //    std::vector<int> originFeatures, refFeatures;
    //    mOriginFPs.GetFPs(originFeatures);
    //    mRefFPs.GetFPs(refFeatures);
    //    int featureSize = originFeatures.size() / 2;
    //    std::vector<cv::Point2f> cvOriginFeatures(featureSize);
    //    std::vector<cv::Point2f> cvRefFeatures(featureSize);
    //    for (int fid = 0; fid < featureSize; fid++)
    //    {
    //        cvOriginFeatures.at(fid).x = originFeatures.at(fid * 2 + 1);
    //        cvOriginFeatures.at(fid).y = originFeatures.at(fid * 2);
    //        cvRefFeatures.at(fid).x = refFeatures.at(fid * 2 + 1);
    //        cvRefFeatures.at(fid).y = refFeatures.at(fid * 2 );
    //    }
    //    cv::Mat transMat = cv::estimateRigidTransform(cvRefFeatures, cvOriginFeatures, false);
    //    DebugLog << "transMat: " << std::endl << transMat << std::endl;
    //    mRefFPTranform.Unit();
    //    mRefFPTranform.SetValue(0, 0, transMat.at<double>(0, 0));
    //    mRefFPTranform.SetValue(0, 1, transMat.at<double>(0, 1));
    //    mRefFPTranform.SetValue(0, 2, transMat.at<double>(0, 2));
    //    mRefFPTranform.SetValue(1, 0, transMat.at<double>(1, 0));
    //    mRefFPTranform.SetValue(1, 1, transMat.at<double>(1, 1));
    //    mRefFPTranform.SetValue(1, 2, transMat.at<double>(1, 2));

    //    std::vector<int> originMark;
    //    mOriginFPs.GetDPs(originMark);
    //    std::vector<int> refMark;
    //    mRefFPs.GetDPs(refMark);
    //    UpdateRightDisplayImage(mImage, &originMark, &refMark, mRefFPTranform);
    //    mUI.UpdateRightImage(mRightDisplayImage);
    //}

    //void FaceBeautificationApp::DeformOriginFace(void)
    //{
    //    std::vector<int> originMarkIndex;
    //    mOriginFPs.GetDPs(originMarkIndex);
    //    std::vector<int> refMarkIndex;
    //    mRefFPs.GetDPs(refMarkIndex);
    //    //Update index w and h
    //    int markSize = originMarkIndex.size() / 2;
    //    for (int mid = 0; mid < markSize; mid++)
    //    {
    //        int temp = originMarkIndex.at(2 * mid);
    //        originMarkIndex.at(2 * mid) = originMarkIndex.at(2 * mid + 1);
    //        originMarkIndex.at(2 * mid + 1) = temp;
    //        //transform ref
    //        double xRes, yRes;
    //        mRefFPTranform.TransformPoint(refMarkIndex.at(2 * mid + 1), refMarkIndex.at(2 * mid), xRes, yRes);
    //        refMarkIndex.at(2 * mid) = xRes;
    //        refMarkIndex.at(2 * mid + 1) = yRes;
    //    }
    //    //Deform
    //    cv::Mat deformImg = MagicDIP::Deformation::DeformByMovingLeastSquares(mImage, originMarkIndex, refMarkIndex);
    //    //UpdateRightDisplayImage(deformImg, &originMarkIndex, &refMarkIndex, mRefFPTranform);
    //    UpdateRightDisplayImage(deformImg, NULL, NULL, mRefFPTranform);
    //    mUI.UpdateRightImage(mRightDisplayImage);
    //}

    //void FaceBeautificationApp::DoPca(void)
    //{
    //    //load feature data
    //    int fileNum = 41;
    //    std::vector<FaceFeaturePoint*> faceFeatureList(fileNum, NULL);
    //    std::vector<MagicMath::HomoMatrix3*> transformList(fileNum, NULL);
    //    for (int fileId = 0; fileId < fileNum; fileId++)
    //    {
    //        std::stringstream ss;
    //        ss << "../../Media/FaceData/f" << fileId << ".fp";
    //        std::string fileName;
    //        ss >> fileName;
    //        faceFeatureList.at(fileId) = new FaceFeaturePoint;
    //        faceFeatureList.at(fileId)->Load(fileName);
    //        transformList.at(fileId) = new MagicMath::HomoMatrix3;
    //        DebugLog << "Load file: " << fileId << " " << fileName << std::endl;
    //        //transformList.at(fileId)->Unit();
    //    }

    //    //align to feature data
    //    std::vector<int> firstfps;
    //    faceFeatureList.at(0)->GetFPs(firstfps);
    //    int pointCount = firstfps.size() / 2;
    //    std::vector<cv::Point2f> cvCurFeatures(pointCount);
    //    std::vector<cv::Point2f> cvRefFeatures(pointCount);
    //    for (int pointId = 0; pointId < pointCount; pointId++)
    //    {
    //        cvRefFeatures.at(pointId).x = firstfps.at(pointId * 2 + 1);
    //        cvRefFeatures.at(pointId).y = firstfps.at(pointId * 2);
    //    }
    //    for (int fileId = 1; fileId < fileNum; fileId++)
    //    {
    //        std::vector<int> curfps;
    //        faceFeatureList.at(fileId)->GetFPs(curfps);
    //        for (int pointId = 0; pointId < pointCount; pointId++)
    //        {
    //            cvCurFeatures.at(pointId).x = curfps.at(pointId * 2 + 1);
    //            cvCurFeatures.at(pointId).y = curfps.at(pointId * 2);
    //        }
    //        cv::Mat transMat = cv::estimateRigidTransform(cvCurFeatures, cvRefFeatures, false);
    //        transformList.at(fileId)->Unit();
    //        transformList.at(fileId)->SetValue(0, 0, transMat.at<double>(0, 0));
    //        transformList.at(fileId)->SetValue(0, 1, transMat.at<double>(0, 1));
    //        transformList.at(fileId)->SetValue(0, 2, transMat.at<double>(0, 2));
    //        transformList.at(fileId)->SetValue(1, 0, transMat.at<double>(1, 0));
    //        transformList.at(fileId)->SetValue(1, 1, transMat.at<double>(1, 1));
    //        transformList.at(fileId)->SetValue(1, 2, transMat.at<double>(1, 2));
    //    }

    //    //calculate mean features
    //    for (int fileId = 1; fileId < fileNum; fileId++)
    //    {
    //        std::vector<int> curfps;
    //        faceFeatureList.at(fileId)->GetFPs(curfps);
    //        for (int pointId = 0; pointId < pointCount; pointId++)
    //        {
    //            double wPosRef = curfps.at(pointId * 2 + 1);
    //            double hPosRef = curfps.at(pointId * 2);
    //            double wPosRes, hPosRes;
    //            transformList.at(fileId)->TransformPoint(wPosRef, hPosRef, wPosRes, hPosRes);
    //            cvRefFeatures.at(pointId).x += wPosRes;
    //            cvRefFeatures.at(pointId).y += hPosRes;
    //        }
    //    }
    //    for (int pointId = 0; pointId < pointCount; pointId++)
    //    {
    //        cvRefFeatures.at(pointId).x /= fileNum;
    //        cvRefFeatures.at(pointId).y /= fileNum;
    //    }

    //    //align to mean features
    //    for (int fileId = 0; fileId < fileNum; fileId++)
    //    {
    //        std::vector<int> curfps;
    //        faceFeatureList.at(fileId)->GetFPs(curfps);
    //        for (int pointId = 0; pointId < pointCount; pointId++)
    //        {
    //            cvCurFeatures.at(pointId).x = curfps.at(pointId * 2 + 1);
    //            cvCurFeatures.at(pointId).y = curfps.at(pointId * 2);
    //        }
    //        cv::Mat transMat = cv::estimateRigidTransform(cvCurFeatures, cvRefFeatures, false);
    //        transformList.at(fileId)->Unit();
    //        transformList.at(fileId)->SetValue(0, 0, transMat.at<double>(0, 0));
    //        transformList.at(fileId)->SetValue(0, 1, transMat.at<double>(0, 1));
    //        transformList.at(fileId)->SetValue(0, 2, transMat.at<double>(0, 2));
    //        transformList.at(fileId)->SetValue(1, 0, transMat.at<double>(1, 0));
    //        transformList.at(fileId)->SetValue(1, 1, transMat.at<double>(1, 1));
    //        transformList.at(fileId)->SetValue(1, 2, transMat.at<double>(1, 2));
    //    }

    //    //display aligned feature
    //    UpdateRightDisplayImage(faceFeatureList, transformList);
    //    mUI.UpdateRightImage(mRightDisplayImage);

    //    //Do Pca
    //    int dataDim = pointCount * 2;
    //    std::vector<double> pcaData(dataDim * fileNum);
    //    for (int fileId = 0; fileId < fileNum; fileId++)
    //    {
    //        std::vector<int> curfps;
    //        faceFeatureList.at(fileId)->GetFPs(curfps);
    //        int baseId = fileId * dataDim;
    //        for (int pointId = 0; pointId < pointCount; pointId++)
    //        {
    //            double wPosRef = curfps.at(pointId * 2 + 1);
    //            double hPosRef = curfps.at(pointId * 2);
    //            double wPosRes, hPosRes;
    //            transformList.at(fileId)->TransformPoint(wPosRef, hPosRef, wPosRes, hPosRes);
    //            pcaData.at(baseId + pointId * 2) = hPosRes;
    //            pcaData.at(baseId + pointId * 2 + 1) = wPosRes;
    //        }
    //    }
    //    int pcaDim = 40;
    //    mPca.Analyse(pcaData, dataDim, pcaDim);

    //    //Save Pca
    //    std::string fileName;
    //    char filterName[] = "PCA Files(*.pca)\0*.pca\0";
    //    if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
    //    {
    //        mPca.Save(fileName);
    //    }

    //    //free memory
    //    for (int fileId = 0; fileId < fileNum; fileId++)
    //    {
    //        delete faceFeatureList.at(fileId);
    //        faceFeatureList.at(fileId) = NULL;
    //        delete transformList.at(fileId);
    //        transformList.at(fileId) = NULL;
    //    }
    //}

    //void FaceBeautificationApp::LoadPca(void)
    //{
    //    std::string fileName;
    //    char filterName[] = "PCA Files(*.pca)\0*.pca\0";
    //    if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
    //    {
    //        mPca.Load(fileName);
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
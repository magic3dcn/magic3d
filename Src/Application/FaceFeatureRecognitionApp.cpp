#include "FaceFeatureRecognitionApp.h"
#include "../AppModules/MagicObjectManager.h"
#include "../AppModules/Face2DObj.h"
#include "../AppModules/FaceFeatureDetection.h"
#include "../Tool/LogSystem.h"
#include "../Common/ToolKit.h"

namespace MagicApp
{
    FaceFeatureRecognitionApp::FaceFeatureRecognitionApp() :
        mMouseMode(MM_View),
        mpF2DObj(NULL),
        mpFfd(NULL),
        mpShapeRegression(NULL)
    {
    }

    FaceFeatureRecognitionApp::~FaceFeatureRecognitionApp()
    {
    }

    bool FaceFeatureRecognitionApp::Enter(void)
    {
        InfoLog << "Enter FaceFeatureRecognition" << std::endl;
        mUI.Setup();
        SetupScene();

        return true;
    }

    bool FaceFeatureRecognitionApp::Update(float timeElapsed)
    {
        return true;
    }

    bool FaceFeatureRecognitionApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();

        return true;
    }

    bool FaceFeatureRecognitionApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        return true;
    }

    bool FaceFeatureRecognitionApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool FaceFeatureRecognitionApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_Mark_KeyPoint)
        {
            int hPos = arg.state.Y.abs - 10;
            int wPos = arg.state.X.abs - 110;
            SynthesizeData(wPos, hPos);
        }
        else if (mMouseMode == MM_Test_KeyPoint)
        {
            int hPos = arg.state.Y.abs - 10;
            int wPos = arg.state.X.abs - 110;
            std::vector<double> initPos(2);
            initPos.at(0) = hPos;
            initPos.at(1) = wPos;
            cv::Mat img = mpF2DObj->GetFaceImage();
            std::vector<double> finalPos;
            mpFfd->PoseRegression(img, initPos, finalPos);
            UpdateDisplayImage(&initPos, &finalPos);
            //UpdateDisplayImage(&initPos, NULL);
        }
        else if (mMouseMode == MM_Test_Shape)
        {
            int hPos = arg.state.Y.abs - 10;
            int wPos = arg.state.X.abs - 110;
            cv::Mat img = mpF2DObj->GetFaceImage();
            std::vector<double> meanFace = mpShapeRegression->GetMeanFace();
            int markSize = meanFace.size() / 2;
            for (int markId = 0; markId < markSize; markId++)
            {
                meanFace.at(markId * 2) += hPos;
                meanFace.at(markId * 2 + 1) += wPos;
            }
            std::vector<double> finalFace;
            mpShapeRegression->ShapeRegression(img, meanFace, finalFace);
            UpdateDisplayImage(&meanFace, &finalFace);
        }

        return true;
    }

    bool FaceFeatureRecognitionApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_K)
        {
            MarkKeyPoint();
        }
        else if (arg.key == OIS::KC_O)
        {
            OpenTestImage();
        }
        else if (arg.key == OIS::KC_S)
        {
            SaveCascadedRegression();
        }
        else if (arg.key == OIS::KC_L)
        {
            LoadCascadedRegression();
        }
        else if (arg.key == OIS::KC_T)
        {
            //TestKeyPoint();
            TestShape();
        }

        return true;
    }

    void FaceFeatureRecognitionApp::WindowResized( Ogre::RenderWindow* rw )
    {

    }

    void FaceFeatureRecognitionApp::SetupScene(void)
    {
        if (!(MOMGR->IsObjExist("Face2DObj")))
        {
            MOMGR->InsertObj("Face2DObj", new Face2DObj);
        }
        mpF2DObj = dynamic_cast<Face2DObj*>(MOMGR->GetObj("Face2DObj"));
        if (!(MOMGR->IsObjExist("CascadedFaceFeatureDetection")))
        {
            MOMGR->InsertObj("CascadedFaceFeatureDetection", new CascadedFaceFeatureDetection);
        }
        mpFfd = dynamic_cast<CascadedFaceFeatureDetection*>(MOMGR->GetObj("CascadedFaceFeatureDetection"));
        if (!(MOMGR->IsObjExist("ShapeRegression")))
        {
            MOMGR->InsertObj("ShapeRegression", new ShapeFaceFeatureDetection);
        }
        mpShapeRegression = dynamic_cast<ShapeFaceFeatureDetection*>(MOMGR->GetObj("ShapeRegression"));
    }

    void FaceFeatureRecognitionApp::ShutdownScene(void)
    {

    }

    void FaceFeatureRecognitionApp::OpenImage(void)
    {
        std::string fileName;
        char filterName[] = "Image Files(*.jpg)\0*.jpg\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpF2DObj->LoadFaceImage(fileName);
            std::string::size_type pos = fileName.rfind(".");
            fileName.replace(pos, 4, ".land");
            mpF2DObj->LoadLandFfp(fileName, mpF2DObj->GetFaceImage().rows);
            std::vector<double> fpsList;
            mpF2DObj->GetFfp()->GetFps(&fpsList);
            UpdateDisplayImage(NULL, &fpsList);
        }
    }

    void FaceFeatureRecognitionApp::LearnCascadedRegression(void)
    {
        std::string fileName;
        char filterName[] = "Land Files(*.txt)\0*.txt\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpFfd->LearnRegression(fileName);
            mpFfd->Save("./simple.cas");
        }
    }

    void FaceFeatureRecognitionApp::SaveCascadedRegression(void)
    {
        std::string fileName;
        char filterName[] = "Cascade Files(*.cas)\0*.cas\0";
        if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
        {
            mpFfd->Save(fileName);
        }
    }

    void FaceFeatureRecognitionApp::LoadCascadedRegression(void)
    {
        std::string fileName;
        char filterName[] = "Cascade Files(*.cas)\0*.cas\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpFfd->Load(fileName);
        }
    }

    void FaceFeatureRecognitionApp::ShapeRegression(void)
    {
        std::string fileName;
        char filterName[] = "Land Files(*.txt)\0*.txt\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpShapeRegression->LearnRegression(fileName);
            mpShapeRegression->Save("./Regression.shape");
        }
    }

    void FaceFeatureRecognitionApp::LoadShapeRegression(void)
    {
        std::string fileName;
        char filterName[] = "Shape Files(*.shape)\0*.shape\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpShapeRegression->Load(fileName);
        }
    }

    void FaceFeatureRecognitionApp::OpenTestImage(void)
    {
        std::string fileName;
        char filterName[] = "Image Files(*.jpg)\0*.jpg\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpF2DObj->LoadFaceImage(fileName);
            UpdateDisplayImage(NULL, NULL);
        }
    }

    void FaceFeatureRecognitionApp::MarkKeyPoint(void)
    {
        if (mMouseMode == MM_Mark_KeyPoint)
        {
            mMouseMode = MM_View;
        }
        else
        {
            mMouseMode = MM_Mark_KeyPoint;
        }
    }

    void FaceFeatureRecognitionApp::SynthesizeData(int keyX, int keyY)
    {
        std::string folderPath = "./Synthesis";
        int shiftSize = 10;
        int fileIndex = 0;
        std::ofstream layoutFout("./Synthesis/LandName.txt");
        layoutFout << (2 * shiftSize + 1) * (2 * shiftSize + 1) << std::endl;
        cv::Mat img = mpF2DObj->GetFaceImage().clone();
        int imgH = img.rows;
        int imgW = img.cols;
        for (int wid = -shiftSize; wid <= shiftSize; wid++)
        {
            for (int hid = -shiftSize; hid <= shiftSize; hid++)
            {
                layoutFout << "/test" << fileIndex << ".land" << std::endl;
                
                std::stringstream ss;
                ss << folderPath << "/test" << fileIndex << ".land";
                std::string landName;
                ss >> landName;
                ss.clear();
                std::ofstream landOut(landName);
                landOut << 1 << std::endl;
                landOut << wid + keyX << " " << imgH - (hid + keyY) << std::endl;
                landOut.close();

                ss << folderPath << "/test" << fileIndex << ".jpg";
                std::string imgName;
                ss >> imgName;
                ss.clear();
                cv::Mat shiftImg(imgH, imgW, CV_8UC3);
                for (int xid = 0; xid < imgW; xid++)
                {
                    for (int yid = 0; yid < imgH; yid++)
                    {
                        int wPos = xid + wid;
                        wPos = wPos < 0 ? 0 : (wPos > imgW - 1 ? imgW - 1 : wPos);
                        int hPos = yid + hid;
                        hPos = hPos < 0 ? 0 : (hPos > imgH - 1 ? imgH - 1 : hPos);
                        unsigned char* shiftPixel = shiftImg.ptr(hPos, wPos);
                        unsigned char* imgPixel = img.ptr(yid, xid);
                        shiftPixel[0] = imgPixel[0];
                        shiftPixel[1] = imgPixel[1];
                        shiftPixel[2] = imgPixel[2];
                    }
                }
                cv::imwrite(imgName, shiftImg);
                shiftImg.release();
                
                fileIndex++;
            }
        }
        layoutFout.close();
    }

    void FaceFeatureRecognitionApp::TestKeyPoint(void)
    {
        if (mMouseMode != MM_Test_KeyPoint)
        {
            mMouseMode = MM_Test_KeyPoint;
        }
        else
        {
            mMouseMode = MM_View;
        }
    }

    void FaceFeatureRecognitionApp::TestShape(void)
    {
        if (mMouseMode != MM_Test_Shape)
        {
            mMouseMode = MM_Test_Shape;
            std::string fileName;
            char filterName[] = "Land Files(*.txt)\0*.txt\0";
            if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
            {
                mpShapeRegression->CalMeanFace(fileName);
            }
        }
        else
        {
            mMouseMode = MM_View;
        }
    }

    void FaceFeatureRecognitionApp::UpdateDisplayImage(const std::vector<double>* dpsList, const std::vector<double>* fpsList)
    {
        cv::Mat displayImg = mpF2DObj->GetFaceImage().clone();
        MarkPointsToImage(displayImg, dpsList, 0, 255, 255, 1);
        MarkPointsToImage(displayImg, fpsList, 255, 255, 0, 1);
        mUI.UpdateImage(displayImg);
        displayImg.release();
    }

    void FaceFeatureRecognitionApp::MarkPointsToImage(cv::Mat& img, const std::vector<double>* markList, 
            unsigned char blue, unsigned char green, unsigned char red, int markWidth)
    {
        if (markList != NULL)
        {
            int imgW = img.cols;
            int imgH = img.rows;
            int markNum = markList->size() / 2;
            for (int mid = 0; mid < markNum; mid++)
            {
                int wPos = floor(markList->at(2 * mid + 1) + 0.5);
                int hPos = floor(markList->at(2 * mid) + 0.5);
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
}

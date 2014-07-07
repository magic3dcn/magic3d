#include "FaceFeatureRecognitionApp.h"
#include "../AppModules/MagicObjectManager.h"
#include "../AppModules/Face2DObj.h"
#include "../Tool/LogSystem.h"
#include "../Common/ToolKit.h"

namespace MagicApp
{
    FaceFeatureRecognitionApp::FaceFeatureRecognitionApp() :
        mpF2DObj(NULL)
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
        return true;
    }

    bool FaceFeatureRecognitionApp::KeyPressed( const OIS::KeyEvent &arg )
    {
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

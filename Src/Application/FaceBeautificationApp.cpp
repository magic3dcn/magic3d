#include "FaceBeautificationApp.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"

namespace MagicApp
{
    FaceFeaturePoint::FaceFeaturePoint() :
        mSelectIndex(-1),
        mSelectType(FT_None)
    {
    }

    FaceFeaturePoint::~FaceFeaturePoint()
    {
    }

    void FaceFeaturePoint::Load()
    {
        std::string fileName;
        char filterName[] = "Feature Point Files(*.fp)\0*.fp\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            std::ifstream fin(fileName);
            int leftEyeNum;
            fin >> leftEyeNum;
            mLeftEyeFPs.clear();
            mLeftEyeFPs.resize(leftEyeNum * 2);
            for (int fid = 0; fid < leftEyeNum; fid++)
            {
                int hid, wid;
                fin >> hid >> wid;
                mLeftEyeFPs.at(fid * 2) = hid;
                mLeftEyeFPs.at(fid * 2 + 1) = wid;
            }
            int rightEyeNum;
            fin >> rightEyeNum;
            mRightEyeFPs.clear();
            mRightEyeFPs.resize(rightEyeNum * 2);
            for (int fid = 0; fid < rightEyeNum; fid++)
            {
                int hid, wid;
                fin >> hid >> wid;
                mRightEyeFPs.at(fid * 2) = hid;
                mRightEyeFPs.at(fid * 2 + 1) = wid;
            }
            int noseNum;
            fin >> noseNum;
            mNoseFPs.clear();
            mNoseFPs.resize(noseNum * 2);
            for (int fid = 0; fid < noseNum; fid++)
            {
                int hid, wid;
                fin >> hid >> wid;
                mNoseFPs.at(fid * 2) = hid;
                mNoseFPs.at(fid * 2 + 1) = wid;
            }
            int mouseNum;
            fin >> mouseNum;
            mMouseFPs.clear();
            mMouseFPs.resize(mouseNum * 2);
            for (int fid = 0; fid < mouseNum; fid++)
            {
                int hid, wid;
                fin >> hid >> wid;
                mMouseFPs.at(fid * 2) = hid;
                mMouseFPs.at(fid * 2 + 1) = wid;
            }
            int borderNum;
            fin >> borderNum;
            mBorderFPs.clear();
            mBorderFPs.resize(borderNum * 2);
            for (int fid = 0; fid < borderNum; fid++)
            {
                int hid, wid;
                fin >> hid >> wid;
                mBorderFPs.at(fid * 2) = hid;
                mBorderFPs.at(fid * 2 + 1) = wid;
            }
            fin.close();
        }
    }

    void FaceFeaturePoint::Save()
    {
        std::string fileName;
        char filterName[] = "Feature Point Files(*.fp)\0*.fp\0";
        if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
        {
            std::ofstream fout(fileName);
            fout << mLeftEyeFPs.size() << " ";
            for (int fid = 0; fid < mLeftEyeFPs.size(); fid++)
            {
                fout << mLeftEyeFPs.at(fid) << " ";
            }
            fout << mRightEyeFPs.size() << " ";
            for (int fid = 0; fid < mRightEyeFPs.size(); fid++)
            {
                fout << mRightEyeFPs.at(fid) << " ";
            }
            fout << mNoseFPs.size() << " ";
            for (int fid = 0; fid < mNoseFPs.size(); fid++)
            {
                fout << mNoseFPs.at(fid) << " ";
            }
            fout << mMouseFPs.size() << " ";
            for (int fid = 0; fid < mMouseFPs.size(); fid++)
            {
                fout << mMouseFPs.at(fid) << " ";
            }
            fout << mBorderFPs.size() << " ";
            for (int fid = 0; fid < mBorderFPs.size(); fid++)
            {
                fout << mBorderFPs.at(fid) << " ";
            }
            fout.close();
        }
    }

    bool FaceFeaturePoint::Select(int hid, int wid)
    {
        int tol = 5;
        for (int fid = 0; fid < mLeftEyeFPs.size() / 2; fid++)
        {
            if (abs(hid - mLeftEyeFPs.at(2 * fid)) < tol &&
                abs(wid - mLeftEyeFPs.at(2 * fid + 1)) < tol)
            {
                mSelectType = FT_Left_Eye;
                mSelectIndex = fid;
                return true;
            }
        }
        for (int fid = 0; fid < mRightEyeFPs.size() / 2; fid++)
        {
            if (abs(hid - mRightEyeFPs.at(2 * fid)) < tol &&
                abs(wid - mRightEyeFPs.at(2 * fid + 1)) < tol)
            {
                mSelectType = FT_Right_Eye;
                mSelectIndex = fid;
                return true;
            }
        }
        for (int fid = 0; fid < mNoseFPs.size() / 2; fid++)
        {
            if (abs(hid - mNoseFPs.at(2 * fid)) < tol &&
                abs(wid - mNoseFPs.at(2 * fid + 1)) < tol)
            {
                mSelectType = FT_Nose;
                mSelectIndex = fid;
                return true;
            }
        }
        for (int fid = 0; fid < mMouseFPs.size() / 2; fid++)
        {
            if (abs(hid - mMouseFPs.at(2 * fid)) < tol &&
                abs(wid - mMouseFPs.at(2 * fid + 1)) < tol)
            {
                mSelectType = FT_Mouse;
                mSelectIndex = fid;
                return true;
            }
        }
        for (int fid = 0; fid < mBorderFPs.size() / 2; fid++)
        {
            if (abs(hid - mBorderFPs.at(2 * fid)) < tol &&
                abs(wid - mBorderFPs.at(2 * fid + 1)) < tol)
            {
                mSelectType = FT_Border;
                mSelectIndex = fid;
                return true;
            }
        }
        return false;
    }

    void FaceFeaturePoint::MoveTo(int hid, int wid)
    {
        if (mSelectType == FT_Left_Eye)
        {
            mLeftEyeFPs.at(mSelectIndex * 2) = hid;
            mLeftEyeFPs.at(mSelectIndex * 2 + 1) = wid;
        }
        else if (mSelectType == FT_Right_Eye)
        {
            mRightEyeFPs.at(mSelectIndex * 2) = hid;
            mRightEyeFPs.at(mSelectIndex * 2 + 1) = wid;
        }
        else if (mSelectType == FT_Nose)
        {
            mNoseFPs.at(mSelectIndex * 2) = hid;
            mNoseFPs.at(mSelectIndex * 2 + 1) = wid;
        }
        else if (mSelectType == FT_Mouse)
        {
            mMouseFPs.at(mSelectIndex * 2) = hid;
            mMouseFPs.at(mSelectIndex * 2 + 1) = wid;
        }
        else if (mSelectType == FT_Border)
        {
            mBorderFPs.at(mSelectIndex * 2) = hid;
            mBorderFPs.at(mSelectIndex * 2 + 1) = wid;
        }
    }

    void FaceFeaturePoint::Get(std::vector<int>& posList)
    {
        posList.clear();
        for (std::vector<int>::iterator itr = mLeftEyeFPs.begin(); itr != mLeftEyeFPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
        for (std::vector<int>::iterator itr = mRightEyeFPs.begin(); itr != mRightEyeFPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
        for (std::vector<int>::iterator itr = mNoseFPs.begin(); itr != mNoseFPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
        for (std::vector<int>::iterator itr = mMouseFPs.begin(); itr != mMouseFPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
        for (std::vector<int>::iterator itr = mBorderFPs.begin(); itr != mBorderFPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
    }

    FaceBeautificationApp::FaceBeautificationApp()
    {
    }

    FaceBeautificationApp::~FaceBeautificationApp()
    {
    }

    bool FaceBeautificationApp::Enter(void)
    {
        InfoLog << "Enter FaceBeautificationApp" << std::endl;
        mUI.Setup();
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
        return true;
    }

    bool FaceBeautificationApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool FaceBeautificationApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool FaceBeautificationApp::KeyPressed( const OIS::KeyEvent &arg )
    {
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

    void FaceBeautificationApp::UpdateLeftDisplayImage(std::vector<int>* markIndex)
    {
        mLeftDisplayImage.release();
        mLeftDisplayImage = mImage.clone();
        if (markIndex != NULL)
        {
            int imgW = mImage.cols;
            int imgH = mImage.rows;
            int markNum = markIndex->size() / 2;
            int markWidth = 3;
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
                        unsigned char* pixel = mLeftDisplayImage.ptr(hid, wid);
                        pixel[0] = 0;
                        pixel[1] = 0;
                        pixel[2] = 255;
                    }
                }
            }
        }
    }

    bool FaceBeautificationApp::OpenImage(void)
    {
        std::string fileName;
        char filterName[] = "Image Files(*.*)\0*.*\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mImage.release();
            mImage = cv::imread(fileName);
            if (mImage.data != NULL)
            {
                UpdateLeftDisplayImage(NULL);
                mUI.UpdateLeftImage(mLeftDisplayImage);
                return true;
            }
        }
        return false;
    }

    void FaceBeautificationApp::LoadFeaturePoint()
    {
        mOriginFPs.Load();
        std::vector<int> markIndex;
        mOriginFPs.Get(markIndex);
        UpdateLeftDisplayImage(&markIndex);
        mUI.UpdateLeftImage(mLeftDisplayImage);
    }

    void FaceBeautificationApp::SaveFeaturePoint()
    {

    }
}
#include "FaceBeautificationApp.h"
#include "../DIP/Deformation.h"
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
            int leftBrowNum;
            fin >> leftBrowNum;
            mLeftBrowFPs.clear();
            mLeftBrowFPs.resize(leftBrowNum * 2);
            for (int fid = 0; fid < leftBrowNum; fid++)
            {
                int hid, wid;
                fin >> hid >> wid;
                mLeftBrowFPs.at(fid * 2) = hid;
                mLeftBrowFPs.at(fid * 2 + 1) = wid;
            }
            int rightBrowNum;
            fin >> rightBrowNum;
            mRightBrowFPs.clear();
            mRightBrowFPs.resize(rightBrowNum * 2);
            for (int fid = 0; fid < rightBrowNum; fid++)
            {
                int hid, wid;
                fin >> hid >> wid;
                mRightBrowFPs.at(fid * 2) = hid;
                mRightBrowFPs.at(fid * 2 + 1) = wid;
            }
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

            UpdateDPs();
        }
    }

    void FaceFeaturePoint::UpdateDPs()
    {
        ConstructOneDPs(mLeftBrowFPs, true, 1, mLeftBrowDPs);
        ConstructOneDPs(mRightBrowFPs, true, 1, mRightBrowDPs);
        ConstructOneDPs(mLeftEyeFPs, true, 1, mLeftEyeDPs);
        ConstructOneDPs(mRightEyeFPs, true, 1, mRightEyeDPs);
        ConstructOneDPs(mNoseFPs, false, 1, mNoseDPs);
        ConstructOneDPs(mMouseFPs, true, 1, mMouseDPs);
        ConstructOneDPs(mBorderFPs, false, 3, mBorderDPs);
    }

    void FaceFeaturePoint::ConstructOneDPs(const std::vector<int>& fps, bool isClosed, int addSize, std::vector<int>& dps)
    {
        double addDelta = 1.0 / (addSize + 1);
        int fpsSize = fps.size() / 2;
        dps.clear();
        for (int fid = 0; fid < fpsSize; fid++)
        {
            int hid = fps.at(fid * 2);
            int wid = fps.at(fid * 2 + 1);
            if (!isClosed && fid == fpsSize - 1)
            {
                dps.push_back(hid);
                dps.push_back(wid);
                break;
            }
            int hidNext = fps.at((fid + 1) % fpsSize * 2);
            int widNext = fps.at((fid + 1) % fpsSize * 2 + 1);
            for (int addId = 0; addId <= addSize; addId++)
            {
                int addHid = int(hid * (1.0 - addId * addDelta) + hidNext * addId * addDelta);
                int addWid = int(wid * (1.0 - addId * addDelta) + widNext * addId * addDelta);
                dps.push_back(addHid);
                dps.push_back(addWid);
            }
        }
    }

    void FaceFeaturePoint::Save()
    {
        std::string fileName;
        char filterName[] = "Feature Point Files(*.fp)\0*.fp\0";
        if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
        {
            std::ofstream fout(fileName);
            fout << mLeftBrowFPs.size() / 2 << " ";
            for (int fid = 0; fid < mLeftBrowFPs.size(); fid++)
            {
                fout << mLeftBrowFPs.at(fid) << " ";
            }
            fout << mRightBrowFPs.size() / 2 << " ";
            for (int fid = 0; fid < mRightBrowFPs.size(); fid++)
            {
                fout << mRightBrowFPs.at(fid) << " ";
            }
            fout << mLeftEyeFPs.size() / 2 << " ";
            for (int fid = 0; fid < mLeftEyeFPs.size(); fid++)
            {
                fout << mLeftEyeFPs.at(fid) << " ";
            }
            fout << mRightEyeFPs.size() / 2 << " ";
            for (int fid = 0; fid < mRightEyeFPs.size(); fid++)
            {
                fout << mRightEyeFPs.at(fid) << " ";
            }
            fout << mNoseFPs.size() / 2 << " ";
            for (int fid = 0; fid < mNoseFPs.size(); fid++)
            {
                fout << mNoseFPs.at(fid) << " ";
            }
            fout << mMouseFPs.size() / 2 << " ";
            for (int fid = 0; fid < mMouseFPs.size(); fid++)
            {
                fout << mMouseFPs.at(fid) << " ";
            }
            fout << mBorderFPs.size() / 2 << " ";
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
        for (int fid = 0; fid < mLeftBrowFPs.size() / 2; fid++)
        {
            if (abs(hid - mLeftBrowFPs.at(2 * fid)) < tol &&
                abs(wid - mLeftBrowFPs.at(2 * fid + 1)) < tol)
            {
                mSelectType = FT_Left_Brow;
                mSelectIndex = fid;
                return true;
            }
        }
        for (int fid = 0; fid < mRightBrowFPs.size() / 2; fid++)
        {
            if (abs(hid - mRightBrowFPs.at(2 * fid)) < tol &&
                abs(wid - mRightBrowFPs.at(2 * fid + 1)) < tol)
            {
                mSelectType = FT_Right_Brow;
                mSelectIndex = fid;
                return true;
            }
        }
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
        if (mSelectType == FT_Left_Brow)
        {
            mLeftBrowFPs.at(mSelectIndex * 2) = hid;
            mLeftBrowFPs.at(mSelectIndex * 2 + 1) = wid;
        }
        else if (mSelectType == FT_Right_Brow)
        {
            mRightBrowFPs.at(mSelectIndex * 2) = hid;
            mRightBrowFPs.at(mSelectIndex * 2 + 1) = wid;
        }
        else if (mSelectType == FT_Left_Eye)
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
        UpdateDPs();
    }

    void FaceFeaturePoint::MoveDelta(int deltaH, int deltaW)
    {
        if (mSelectType == FT_Left_Brow)
        {
            mLeftBrowFPs.at(mSelectIndex * 2) += deltaH;
            mLeftBrowFPs.at(mSelectIndex * 2 + 1) += deltaW;
        }
        else if (mSelectType == FT_Right_Brow)
        {
            mRightBrowFPs.at(mSelectIndex * 2) += deltaH;
            mRightBrowFPs.at(mSelectIndex * 2 + 1) += deltaW;
        }
        else if (mSelectType == FT_Left_Eye)
        {
            mLeftEyeFPs.at(mSelectIndex * 2) += deltaH;
            mLeftEyeFPs.at(mSelectIndex * 2 + 1) += deltaW;
        }
        else if (mSelectType == FT_Right_Eye)
        {
            mRightEyeFPs.at(mSelectIndex * 2) += deltaH;
            mRightEyeFPs.at(mSelectIndex * 2 + 1) += deltaW;
        }
        else if (mSelectType == FT_Nose)
        {
            mNoseFPs.at(mSelectIndex * 2) += deltaH;
            mNoseFPs.at(mSelectIndex * 2 + 1) += deltaW;
        }
        else if (mSelectType == FT_Mouse)
        {
            mMouseFPs.at(mSelectIndex * 2) += deltaH;
            mMouseFPs.at(mSelectIndex * 2 + 1) += deltaW;
        }
        else if (mSelectType == FT_Border)
        {
            mBorderFPs.at(mSelectIndex * 2) += deltaH;
            mBorderFPs.at(mSelectIndex * 2 + 1) += deltaW;
        }
        UpdateDPs();
    }

    void FaceFeaturePoint::GetDPs(std::vector<int>& posList)
    {
        posList.clear();
        for (std::vector<int>::iterator itr = mLeftBrowDPs.begin(); itr != mLeftBrowDPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
        for (std::vector<int>::iterator itr = mRightBrowDPs.begin(); itr != mRightBrowDPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
        for (std::vector<int>::iterator itr = mLeftEyeDPs.begin(); itr != mLeftEyeDPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
        for (std::vector<int>::iterator itr = mRightEyeDPs.begin(); itr != mRightEyeDPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
        for (std::vector<int>::iterator itr = mNoseDPs.begin(); itr != mNoseDPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
        for (std::vector<int>::iterator itr = mMouseDPs.begin(); itr != mMouseDPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
        for (std::vector<int>::iterator itr = mBorderDPs.begin(); itr != mBorderDPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
    }

    void FaceFeaturePoint::GetFPs(std::vector<int>& posList)
    {
        posList.clear();
        for (std::vector<int>::iterator itr = mLeftBrowFPs.begin(); itr != mLeftBrowFPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
        for (std::vector<int>::iterator itr = mRightBrowFPs.begin(); itr != mRightBrowFPs.end(); itr++)
        {
            posList.push_back(*itr);
        }
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

    void FaceFeaturePoint::GetTransform(double& posX, double& posY, double& dirX, double& dirY, double& scale)
    {
        double leftPosX = 0;
        double leftPosY = 0;
        double rightPosX = 0;
        double rightPosY = 0;
        int browSize = mLeftBrowFPs.size() / 2;
        for (int fid = 0; fid < browSize; fid++)
        {
            leftPosX += mLeftBrowFPs.at(2 * fid + 1);
            leftPosY += mLeftBrowFPs.at(2 * fid);
            rightPosX += mRightBrowFPs.at(2 * fid + 1);
            rightPosY += mRightBrowFPs.at(2 * fid);
        }
        int eyeSize = mLeftEyeFPs.size() / 2;
        for (int fid = 0; fid < eyeSize; fid++)
        {
            leftPosX += mLeftEyeFPs.at(2 * fid + 1);
            leftPosY += mLeftEyeFPs.at(2 * fid);
            rightPosX += mRightEyeFPs.at(2 * fid + 1);
            rightPosY += mRightEyeFPs.at(2 * fid);
        }
        int borderRevIndex = mBorderFPs.size() - 1;
        int halfBorderSize = mBorderFPs.size() / 2 / 2;
        for (int fid = 0; fid < halfBorderSize; fid++)
        {
            leftPosX += mBorderFPs.at(2 * fid + 1);
            leftPosY += mBorderFPs.at(2 * fid);
            rightPosX += mBorderFPs.at(borderRevIndex - 2 * fid);
            rightPosY += mBorderFPs.at(borderRevIndex - 2 * fid - 1);
        }
        int statSize = browSize + eyeSize + halfBorderSize;
        leftPosX = leftPosX / statSize;
        leftPosY = leftPosY / statSize;
        rightPosX = rightPosX / statSize;
        rightPosY = rightPosY / statSize;
        posX = (leftPosX + rightPosX) / 2;
        posY = (leftPosY + rightPosY) / 2;
        dirX = leftPosY - rightPosY;
        dirY = rightPosX - leftPosX;
        scale = sqrt(dirX * dirX + dirY * dirY);
        dirX /= scale;
        dirY /= scale;
    }

    FaceBeautificationApp::FaceBeautificationApp() :
        mFeaturePointSelected(false),
        mMouseMode(MM_View)
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
        if (mMouseMode == MM_Move_Origin_Feature)
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
                int tol = 5;
                mFeaturePointSelected = mOriginFPs.Select(hPos, wPos);
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
            mOriginFPs.MoveDelta(deltaH, deltaW);
            std::vector<int> markIndex;
            mOriginFPs.GetDPs(markIndex);
            std::vector<int> featureIndex;
            mOriginFPs.GetFPs(featureIndex);
            UpdateLeftDisplayImage(&markIndex, &featureIndex);
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

    void FaceBeautificationApp::UpdateLeftDisplayImage(const std::vector<int>* markIndex, const std::vector<int>* featureIndex)
    {
        mLeftDisplayImage.release();
        mLeftDisplayImage = mImage.clone();
        if (markIndex != NULL)
        {
            int imgW = mImage.cols;
            int imgH = mImage.rows;
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
                        unsigned char* pixel = mLeftDisplayImage.ptr(hid, wid);
                        pixel[0] = 0;
                        pixel[1] = 0;
                        pixel[2] = 255;
                    }
                }
            }
        }
        if (featureIndex != NULL)
        {
            int imgW = mImage.cols;
            int imgH = mImage.rows;
            int markNum = featureIndex->size() / 2;
            int markWidth = 1;
            for (int mid = 0; mid < markNum; mid++)
            {
                int wPos = featureIndex->at(2 * mid + 1);
                int hPos = featureIndex->at(2 * mid);
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
                        pixel[1] = 255;
                        pixel[2] = 255;
                    }
                }
            }
        }
    }

    void FaceBeautificationApp::UpdateMidDisplayImage(const std::vector<int>* markIndex)
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
                cv::Mat resizedImg = ResizeInputImageToCanvas(mImage);
                mImage.release();
                mImage = resizedImg;
                std::vector<int> markIndex;
                mOriginFPs.GetDPs(markIndex);
                UpdateLeftDisplayImage(&markIndex, NULL);
                mUI.UpdateLeftImage(mLeftDisplayImage);

                std::vector<int> refMarkIndex;
                mRefFPs.GetDPs(refMarkIndex);
                UpdateRightDisplayImage(mImage, &markIndex, &refMarkIndex, mRefFPTranform);
                mUI.UpdateRightImage(mRightDisplayImage);

                mMouseMode = MM_View;
                return true;
            }
        }
        return false;
    }

    void FaceBeautificationApp::LoadFeaturePoint()
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
    }

    void FaceBeautificationApp::MoveOriginFeaturePoint()
    {
        mMouseMode = MM_Move_Origin_Feature;
        std::vector<int> markIndex;
        mOriginFPs.GetDPs(markIndex);
        std::vector<int> featureIndex;
        mOriginFPs.GetFPs(featureIndex);
        UpdateLeftDisplayImage(&markIndex, &featureIndex);
        mUI.UpdateLeftImage(mLeftDisplayImage);
    }

    void FaceBeautificationApp::SaveFeaturePoint()
    {
        mOriginFPs.Save();
    }

    bool FaceBeautificationApp::OpenRefImage(void)
    {
        std::string fileName;
        char filterName[] = "Image Files(*.*)\0*.*\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mRefImage.release();
            mRefImage = cv::imread(fileName);
            if (mRefImage.data != NULL)
            {
                mRefImage = ResizeInputImageToCanvas(mRefImage);
                std::vector<int> markIndex;
                mRefFPs.GetDPs(markIndex);
                UpdateMidDisplayImage(&markIndex);
                mUI.UpdateMiddleImage(mMidDisplayImage);

                return true;
            }
        }
        return false;
    }

    void FaceBeautificationApp::LoadRefFeaturePoint(void)
    {
        mRefFPs.Load();
        std::vector<int> refMarkIndex;
        mRefFPs.GetDPs(refMarkIndex);
        UpdateMidDisplayImage(&refMarkIndex);
        mUI.UpdateMiddleImage(mMidDisplayImage);

        std::vector<int> originMarkIndex;
        mOriginFPs.GetDPs(originMarkIndex);
        UpdateRightDisplayImage(mImage, &originMarkIndex, &refMarkIndex, mRefFPTranform);
        mUI.UpdateRightImage(mRightDisplayImage);
    }

    void FaceBeautificationApp::AlignFeature(void)
    {
        double originPosX, originPosY, originDirX, originDirY, originScale;
        mOriginFPs.GetTransform(originPosX, originPosY, originDirX, originDirY, originScale);
        double refPosX, refPosY, refDirX, refDirY, refScale;
        mRefFPs.GetTransform(refPosX, refPosY, refDirX, refDirY, refScale);
        MagicMath::HomoMatrix3 translateToCenterMat;
        translateToCenterMat.GenerateTranslation(-refPosX, -refPosY);
        double cosTheta = originDirX * refDirX + originDirY * refDirY;
        cosTheta = cosTheta > 1 ? 1 : cosTheta;
        double theta = acos(cosTheta);
        double flag = refDirX * originDirY - originDirX * refDirY;
        if (flag < 0)
        {
            theta *= -1;
        }
        MagicMath::HomoMatrix3 rotateMat;
        rotateMat.GenerateRotation(theta);
        MagicMath::HomoMatrix3 scaleMat;
        scaleMat.GenerateScaling(originScale / refScale, originScale / refScale);
        MagicMath::HomoMatrix3 translateToOriginMat;
        translateToOriginMat.GenerateTranslation(originPosX, originPosY);
        mRefFPTranform = translateToOriginMat * scaleMat * rotateMat * translateToCenterMat;
        //fmRefFPTranform = scaleMat * translateMat;
        std::vector<int> originMark;
        mOriginFPs.GetDPs(originMark);
        std::vector<int> refMark;
        mRefFPs.GetDPs(refMark);
        UpdateRightDisplayImage(mImage, &originMark, &refMark, mRefFPTranform);
        mUI.UpdateRightImage(mRightDisplayImage);
    }

    void FaceBeautificationApp::DeformOriginFace(void)
    {
        std::vector<int> originMarkIndex;
        mOriginFPs.GetDPs(originMarkIndex);
        std::vector<int> refMarkIndex;
        mRefFPs.GetDPs(refMarkIndex);
        //Update index w and h
        int markSize = originMarkIndex.size() / 2;
        for (int mid = 0; mid < markSize; mid++)
        {
            int temp = originMarkIndex.at(2 * mid);
            originMarkIndex.at(2 * mid) = originMarkIndex.at(2 * mid + 1);
            originMarkIndex.at(2 * mid + 1) = temp;
            //transform ref
            double xRes, yRes;
            mRefFPTranform.TransformPoint(refMarkIndex.at(2 * mid + 1), refMarkIndex.at(2 * mid), xRes, yRes);
            refMarkIndex.at(2 * mid) = xRes;
            refMarkIndex.at(2 * mid + 1) = yRes;
        }
        //Deform
        cv::Mat deformImg = MagicDIP::Deformation::DeformByMovingLeastSquares(mImage, originMarkIndex, refMarkIndex);
        //UpdateRightDisplayImage(deformImg, &originMarkIndex, &refMarkIndex, mRefFPTranform);
        UpdateRightDisplayImage(deformImg, NULL, NULL, mRefFPTranform);
        mUI.UpdateRightImage(mRightDisplayImage);
    }

    cv::Mat FaceBeautificationApp::ResizeInputImageToCanvas(const cv::Mat& img) const
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
    }
}
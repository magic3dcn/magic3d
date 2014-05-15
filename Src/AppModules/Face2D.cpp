#include "Face2D.h"
#include <fstream>

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

    /*void FaceFeaturePoint::Load()
    {
        std::string fileName;
        char filterName[] = "Feature Point Files(*.fp)\0*.fp\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            Load(fileName);
        }
    }*/

    bool FaceFeaturePoint::Load(const std::string& fileName)
    {
        std::ifstream fin(fileName);
        if (!fin.is_open())
        {
            return false;
        }
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

        return true;
    }

    void FaceFeaturePoint::Load(int browNum, int eyeNum, int noseNum, int mouseNum, int borderNum, const std::vector<int>& posList)
    {
        int baseIndex = 0;
        mLeftBrowFPs.clear();
        mLeftBrowFPs.resize(browNum * 2);
        for (int pointId = 0; pointId < browNum; pointId++)
        {
            mLeftBrowFPs.at(pointId * 2) = posList.at(baseIndex + pointId * 2);
            mLeftBrowFPs.at(pointId * 2 + 1) = posList.at(baseIndex + pointId * 2 + 1);
        }
        baseIndex += (browNum * 2);

        mRightBrowFPs.clear();
        mRightBrowFPs.resize(browNum * 2);
        for (int pointId = 0; pointId < browNum; pointId++)
        {
            mRightBrowFPs.at(pointId * 2) = posList.at(baseIndex + pointId * 2);
            mRightBrowFPs.at(pointId * 2 + 1) = posList.at(baseIndex + pointId * 2 + 1);
        }
        baseIndex += (browNum * 2);

        mLeftEyeFPs.clear();
        mLeftEyeFPs.resize(eyeNum * 2);
        for (int pointId = 0; pointId < eyeNum; pointId++)
        {
            mLeftEyeFPs.at(pointId * 2) = posList.at(baseIndex + pointId * 2);
            mLeftEyeFPs.at(pointId * 2 + 1) = posList.at(baseIndex + pointId * 2 + 1);
        }
        baseIndex += (eyeNum * 2);

        mRightEyeFPs.clear();
        mRightEyeFPs.resize(eyeNum * 2);
        for (int pointId = 0; pointId < eyeNum; pointId++)
        {
            mRightEyeFPs.at(pointId * 2) = posList.at(baseIndex + pointId * 2);
            mRightEyeFPs.at(pointId * 2 + 1) = posList.at(baseIndex + pointId * 2 + 1);
        }
        baseIndex += (eyeNum * 2);

        mNoseFPs.clear();
        mNoseFPs.resize(noseNum * 2);
        for (int pointId = 0; pointId < noseNum; pointId++)
        {
            mNoseFPs.at(pointId * 2) = posList.at(baseIndex + pointId * 2);
            mNoseFPs.at(pointId * 2 + 1) = posList.at(baseIndex + pointId * 2 + 1);
        }
        baseIndex += (noseNum * 2);

        mMouseFPs.clear();
        mMouseFPs.resize(mouseNum * 2);
        for (int pointId = 0; pointId < mouseNum; pointId++)
        {
            mMouseFPs.at(pointId * 2) = posList.at(baseIndex + pointId * 2);
            mMouseFPs.at(pointId * 2 + 1) = posList.at(baseIndex + pointId * 2 + 1);
        }
        baseIndex += (mouseNum * 2);

        mBorderFPs.clear();
        mBorderFPs.resize(borderNum * 2);
        for (int pointId = 0; pointId < borderNum; pointId++)
        {
            mBorderFPs.at(pointId * 2) = posList.at(baseIndex + pointId * 2);
            mBorderFPs.at(pointId * 2 + 1) = posList.at(baseIndex + pointId * 2 + 1);
        }
        baseIndex += (borderNum * 2);

        UpdateDPs();
    }

    void FaceFeaturePoint::UpdateDPs()
    {
        ConstructOneDPs(mLeftBrowFPs, true, 2, mLeftBrowDPs);
        ConstructOneDPs(mRightBrowFPs, true, 2, mRightBrowDPs);
        ConstructOneDPs(mLeftEyeFPs, true, 2, mLeftEyeDPs);
        ConstructOneDPs(mRightEyeFPs, true, 2, mRightEyeDPs);
        ConstructOneDPs(mNoseFPs, false, 2, mNoseDPs);
        ConstructOneDPs(mMouseFPs, true, 2, mMouseDPs);
        ConstructOneDPs(mBorderFPs, false, 10, mBorderDPs);
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

    void FaceFeaturePoint::Save(const std::string& fileName)
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

        mSelectType = FT_None;
        mSelectIndex = -1;
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

    void FaceFeaturePoint::GetParameter(int& browNum, int& eyeNum, int& noseNum, int& mouseNum, int& borderNum)
    {
        browNum = mLeftBrowFPs.size() / 2;
        eyeNum = mLeftEyeFPs.size() / 2;
        noseNum = mNoseFPs.size() / 2;
        mouseNum = mMouseFPs.size() / 2;
        borderNum = mBorderFPs.size() / 2;
    }

    Face2D::Face2D()
    {
    }

    Face2D::~Face2D()
    {
        mImage.release();
        mRefImage.release();
    }

    bool Face2D::LoadImage(const std::string& fileName)
    {
        mImage.release();
        mImage = cv::imread(fileName);
        if (mImage.data != NULL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    cv::Mat Face2D::GetImage(void)
    {
        return mImage;
    }

    bool Face2D::LoadFps(const std::string& fileName)
    {
        return mFps.Load(fileName);
    }

    FaceFeaturePoint* Face2D::GetFps(void)
    {
        return &mFps;
    }

    bool Face2D::LoadRefImage(const std::string& fileName)
    {
        mRefImage.release();
        mRefImage = cv::imread(fileName);
        if (mRefImage.data != NULL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    cv::Mat Face2D::GetRefImage(void)
    {
        return mRefImage;
    }

    bool Face2D::LoadrefFps(const std::string& fileName)
    {
        return mRefFps.Load(fileName);
    }

    FaceFeaturePoint* Face2D::GetRefFps(void)
    {
        return &mRefFps;
    }

    void Face2D::SetMaxImageSize(int width, int height)
    {
        int imgW = mImage.cols;
        int imgH = mImage.rows;
        bool resized = false;
        if (imgW > width)
        {
            imgH = int(imgH * float(width) / imgW);
            imgW = width;
            resized = true;
        }
        if (imgH > height)
        {
            imgW = int(imgW * float(height) / imgH);
            imgH = height;
            resized = true;
        }
        if (resized)
        {
            cv::Size vcSize(imgW, imgH);
            cv::Mat resizedImg(vcSize, CV_8UC3);
            cv::resize(mImage, resizedImg, vcSize);
            mImage.release();
            mImage = resizedImg.clone();
        }
    }

    void Face2D::SetMaxRefImageSize(int width, int height)
    {
        int imgW = mRefImage.cols;
        int imgH = mRefImage.rows;
        bool resized = false;
        if (imgW > width)
        {
            imgH = int(imgH * float(width) / imgW);
            imgW = width;
            resized = true;
        }
        if (imgH > height)
        {
            imgW = int(imgW * float(height) / imgH);
            imgH = height;
            resized = true;
        }
        if (resized)
        {
            cv::Size vcSize(imgW, imgH);
            cv::Mat resizedImg(vcSize, CV_8UC3);
            cv::resize(mRefImage, resizedImg, vcSize);
            mRefImage.release();
            mRefImage = resizedImg.clone();
        }
    }
}

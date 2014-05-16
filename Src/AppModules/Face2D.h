#pragma once
#include <vector>
#include <string>
#include "opencv2/opencv.hpp"
#include "../MachineLearning/PrincipalComponentAnalysis.h"

namespace MagicApp
{
    class FaceFeaturePoint
    {
    public:
        enum FeatureType
        {
            FT_None = 0,
            FT_Left_Brow,
            FT_Right_Brow,
            FT_Left_Eye,
            FT_Right_Eye,
            FT_Mouse,
            FT_Nose,
            FT_Border
        };

        FaceFeaturePoint();
        
        //void Load();
        bool Load(const std::string& fileName);
        void Load(int browNum, int eyeNum, int noseNum, int mouseNum, int borderNum, const std::vector<int>& posList);
        void Save(const std::string& fileName);
        bool Select(int hid, int wid);
        void MoveTo(int hid, int wid);
        void MoveDelta(int deltaH, int deltaW);
        void GetDPs(std::vector<int>& posList); // h, w
        void GetFPs(std::vector<int>& posList);
        void GetParameter(int& browNum, int& eyeNum, int& noseNum, int& mouseNum, int& borderNum);

        ~FaceFeaturePoint();

    private:
        void UpdateDPs();
        void ConstructOneDPs(const std::vector<int>& fps, bool isClosed, int addSize, std::vector<int>& dps);

    private:
        int mSelectIndex;
        FeatureType mSelectType;
        std::vector<int> mLeftBrowFPs;
        std::vector<int> mLeftBrowDPs;
        std::vector<int> mRightBrowFPs;
        std::vector<int> mRightBrowDPs;
        std::vector<int> mLeftEyeFPs;
        std::vector<int> mLeftEyeDPs;
        std::vector<int> mRightEyeFPs;
        std::vector<int> mRightEyeDPs;
        std::vector<int> mNoseFPs;
        std::vector<int> mNoseDPs;
        std::vector<int> mMouseFPs;
        std::vector<int> mMouseDPs;
        std::vector<int> mBorderFPs;
        std::vector<int> mBorderDPs;
    };

    class Face2D
    {
    public:
        Face2D();
        ~Face2D();

        bool LoadImage(const std::string& fileName);
        cv::Mat GetImage(void);
        void GetImageSize(int* imgW, int* imgH);
        bool LoadFps(const std::string& fileName);
        FaceFeaturePoint* GetFps(void);
        bool LoadRefImage(const std::string& fileName);
        cv::Mat GetRefImage(void);
        bool LoadRefFps(const std::string& fileName);
        FaceFeaturePoint* GetRefFps(void);
        cv::Mat DeformImageByFeature(void);
        void SetMaxImageSize(int width, int height);
        void SetMaxRefImageSize(int width, int height);
        void DoFeaturePca(const std::string& path, int imgCount);

    private:
        cv::Mat* mpImage;
        FaceFeaturePoint* mpFps;
        cv::Mat* mpRefImage;
        FaceFeaturePoint* mpRefFps;
        MagicML::PrincipalComponentAnalysis* mpFeaturePca;
    };

}

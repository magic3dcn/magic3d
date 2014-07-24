#pragma once
#include "MagicObject.h"
#include "opencv2/opencv.hpp"
#include <string>
#include <vector>

namespace MagicDIP
{
    class RealTimeFaceDetection;
}

namespace MagicApp
{
    class FaceDetection : public MagicObject
    {
    public:
        enum DetectionMethod
        {
            DM_Default
        };

        FaceDetection();
        ~FaceDetection();

        int LearnDetector(const std::string& imgFile, DetectionMethod dm);
        int DetectFace(const cv::Mat& img, std::vector<int>& faces);
        void Save(const std::string& fileName) const;
        void Load(const std::string& fileName);

    private:
        int LearnRealTimeDetector(const std::string& imgFile);

    private:
        DetectionMethod mDm;
        MagicDIP::RealTimeFaceDetection* mpRealTimeDetector;
    };

    
}

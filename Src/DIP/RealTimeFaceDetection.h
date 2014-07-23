#pragma once
#include <vector>
#include <string>
#include "opencv2/opencv.hpp"
#include "ImageLoader.h"

namespace MagicDIP
{
    struct HaarFeature
    {
        int sRow;
        int sCol;
        int lRow;
        int lCol;
        int type;
    };

    class HaarClassifier
    {
    public:
        HaarClassifier();
        ~HaarClassifier();

        int Learn(const ImageLoader& imgLoader, const std::vector<double>& dataWeights, const std::vector<int>& dataY, 
            double* trainError);
        int Predict(const cv::Mat& img, int sRow, int sCol, float scale) const;
        int Predict(const ImageLoader& imgLoader, int dataId) const;

    private:
        HaarFeature mFeature;
        int mThreshold;
        bool mIsLess;
        //cache
    };

    class AdaBoostFaceDetection
    {
    public:
        AdaBoostFaceDetection();
        ~AdaBoostFaceDetection();

        int Learn(const ImageLoader& imgLoader, const std::vector<int>& dataY, int levelCount);
        int Predict(const cv::Mat& img, int sRow, int sCol, float scale) const;

    private:
        HaarClassifier* TrainWeakClassifier(const ImageLoader& imgLoader, const std::vector<double>& dataWeights,
            const std::vector<int>& dataY) const;
        void Reset(void);

    private:
        std::vector<HaarClassifier*> mClassifiers;
        std::vector<double> mClassifierWeights;
        double mThreshold;
    };

    class RealTimeFaceDetection
    {
    public:
        RealTimeFaceDetection();
        ~RealTimeFaceDetection();

        int Learn(const std::vector<std::string>& imgFiles);

    private:

    };
}

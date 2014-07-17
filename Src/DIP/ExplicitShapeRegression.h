#pragma once
#include <vector>
#include <string>
#include "opencv2/opencv.hpp"

namespace MagicDIP
{
    class ExplicitShapeRegression
    {
    public:
        ExplicitShapeRegression();
        ~ExplicitShapeRegression();

        int LearnRegression(const std::vector<std::string>& imgFiles, const std::vector<double>& initTheta, 
            const std::vector<double>& finalTheta, int keyPointCount, int outerCount, int innerCount, int fernSize, int featureSize);
        int ShapeRegression(const cv::Mat& img, const std::vector<double>& initTheta, std::vector<double>&finalTheta) const;
        void Save(const std::string& fileName) const;
        void Load(const std::string& fileName);

    private:

    };
}

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

        int LearnRegression();
        int ShapeRegression(const cv::Mat& img, const std::vector<double>& initTheta, std::vector<double>&finalTheta) const;
        virtual void Save(const std::string& fileName) const = 0;
        virtual void Load(const std::string& fileName) = 0;

    private:

    };
}

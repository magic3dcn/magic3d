#pragma once
#include "opencv2/opencv.hpp"
#include <vector>

namespace MagicDIP
{
    class Deformation
    {
    public:
        Deformation();
        ~Deformation();

        static cv::Mat DeformByMovingLeastSquares(const cv::Mat& inputImg, 
            const std::vector<int>& originIndex, const std::vector<int>& targetIndex);

    };

}

#pragma once
#include "opencv2/opencv.hpp"

namespace MagicDIP
{
    class Segmentation
    {
    public:
        Segmentation();
        ~Segmentation();

        static cv::Mat SegmentByGraphCut(const cv::Mat& inputImg, const cv::Mat& markImg);
    };

}

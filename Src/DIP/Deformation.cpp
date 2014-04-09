#include "Deformation.h"

namespace MagicDIP
{
    Deformation::Deformation()
    {
    }

    Deformation::~Deformation()
    {
    }

    cv::Mat Deformation::DeformByMovingLeastSquares(const cv::Mat& inputImg, 
            const std::vector<int>& originIndex, const std::vector<int>& targetIndex)
    {
        int imgW = inputImg.cols;
        int imgH = inputImg.rows;
        cv::Size imgSize(imgW, imgH);
        cv::Mat resImg(imgSize, CV_8UC3);
        resImg = inputImg.clone();
        return resImg;
    }
}

#pragma once
#include "opencv2/opencv.hpp"

namespace MagicDIP
{
    class Retargetting
    {
    public:
        Retargetting();
        ~Retargetting();

        static cv::Mat SeamCarvingResizing(const cv::Mat& inputImg, int targetW, int targetH);
        static cv::Mat FastSeamCarvingResizing(const cv::Mat& inputImg, int targetW, int targetH);

    private:
        static void SeamCarvingHorizontal(cv::Mat& img, int originW, int originH, int targetW);
        static void SeamCarvingVertical(cv::Mat& img, int originW, int originH, int targetH);
        static void FastSeamCarvingHorizontal(cv::Mat& img, int originW, int originH, int targetW);
        static void FastSeamCarvingVertical(cv::Mat& img, int originW, int originH, int targetH);
        static void FindMinimalK(const std::vector<int>& scores, int k, std::vector<int>& minimalK);
    };
}
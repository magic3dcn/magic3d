#include "Saliency.h"

namespace MagicDIP
{
    SaliencyDetection::SaliencyDetection()
    {
    }

    SaliencyDetection::~SaliencyDetection()
    {
    }

    cv::Mat SaliencyDetection::DoGBandSaliency(const cv::Mat& inputImg)
    {
        cv::Mat cvtImg;
        cv::cvtColor(inputImg, cvtImg, CV_RGB2Lab);
        //cv::cvtColor(inputImg, cvtImg, CV_BGR2Lab);
        int inputW = inputImg.cols;
        int inputH = inputImg.rows;
        for (int hid = 0; hid < inputH; hid++)
        {
            for (int wid = 0; wid < inputW; wid++)
            {
                unsigned char* pixel = cvtImg.ptr(hid, wid);
                pixel[0] = pixel[2];
                pixel[1] = pixel[2];
            }
        }
        return cvtImg;
    }
}

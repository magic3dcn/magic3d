#include "Segmentation.h"

namespace MagicDIP
{
    Segmentation::Segmentation()
    {
    }

    Segmentation::~Segmentation()
    {
    }

    cv::Mat Segmentation::SegmentByGraphCut(const cv::Mat& inputImg, const cv::Mat& markImg)
    {
        int imgW = inputImg.cols;
        int imgH = inputImg.rows;
        cv::Size imgSize(imgW, imgH);
        cv::Mat segImg(imgSize, CV_8UC3);
        
        for (int hid = 0; hid < imgH; hid++)
        {
            for (int wid = 0; wid < imgW; wid++)
            {
                const unsigned char* pixel = markImg.ptr(hid, wid);
                if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0)
                {
                    unsigned char* segPixel = segImg.ptr(hid, wid);
                    segPixel[0] = 255;
                    segPixel[1] = 0;
                    segPixel[2] = 0;
                }
            }
        }

        return segImg;
    }
}
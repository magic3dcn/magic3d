#include "Retargetting.h"
#include <vector>
#include <math.h>
#include "../Common/ToolKit.h"

namespace MagicDIP
{
    Retargetting::Retargetting()
    {
    }

    Retargetting::~Retargetting()
    {
    }

    cv::Mat Retargetting::SeamCarvingResizing(const cv::Mat& inputImg, int targetW, int targetH)
    {
        int inputW = inputImg.cols;
        int inputH = inputImg.rows;
        cv::Mat imgPro;
        int horizontalTime = inputW - targetW;
        if (horizontalTime > 0)
        {
            imgPro = inputImg.clone();
            SeamCarvingHorizontal(imgPro, inputW, inputH, targetW);
        }
        else if (horizontalTime < 0)
        {
            //not do
            
        }
        else
        {
            imgPro = inputImg.clone();
        }
        
        int verticalTime = inputH - targetH;
        if (verticalTime > 0)
        {
            SeamCarvingVertical(imgPro, targetW, inputH, targetH);
            cv::Mat imgRes = imgPro.rowRange(0, targetH - 1).colRange(0, targetW - 1);
            return imgRes;
        }
        else if (verticalTime < 0)
        {
            //not do
            return imgPro;
        }
        else
        {
            cv::Mat imgRes = imgPro.colRange(0, targetW - 1);
            return imgRes;
        }
    }

    void Retargetting::SeamCarvingHorizontal(cv::Mat& img, int originW, int originH, int targetW)
    {
        int cutTimes = originW - targetW;
        std::vector<std::vector<int> > gradMat(originH);
        for (int hid = 0; hid < originH; hid++)
        {
            std::vector<int> gradList(originW);
            for (int wid = 0; wid < originW; wid++)
            {
                unsigned char* pixel = img.ptr(hid, wid);
                unsigned char* pixelWNext = NULL;
                if (wid == 0)
                {
                    pixelWNext = img.ptr(hid, wid + 1);
                }
                else
                {
                    pixelWNext = img.ptr(hid, wid - 1);
                }
                unsigned char* pixelHNext = NULL;
                if (hid == 0)
                {
                    pixelHNext = img.ptr(hid + 1, wid);
                }
                else
                {
                    pixelHNext = img.ptr(hid - 1, wid);
                }
                gradList.at(wid) = abs(pixel[0] - pixelWNext[0]) + abs(pixel[1] - pixelWNext[1]) + abs(pixel[2] - pixelWNext[2]) +
                    abs(pixel[0] - pixelHNext[0]) + abs(pixel[1] - pixelHNext[1]) + abs(pixel[2] - pixelHNext[2]);
            }
            gradMat.at(hid) = gradList;
        }
        std::vector<std::vector<int> > traceIndex(originH);
        std::vector<int> curM(originW);
        std::vector<int> lastM(originW, 0);
        std::vector<int> traceLastIndex(originW);
        for (int cutIndex = 0; cutIndex < cutTimes; cutIndex++)
        {
            int curW = originW - cutIndex;
            for (int hid = 0; hid < originH; hid++)
            {
                std::vector<int>& gradList = gradMat.at(hid);
                //wid == 0
                {
                    if (lastM.at(1) < lastM.at(0))
                    {
                        curM.at(0) = gradList.at(0) + lastM.at(1);
                        traceLastIndex.at(0) = 1;
                    }
                    else
                    {
                        curM.at(0) = gradList.at(0) + lastM.at(0);
                        traceLastIndex.at(0) = 0;
                    }
                }
                //0 < wid < curW - 1
                for (int wid = 1; wid < curW - 1; wid++)
                {
                    int traceTemp = wid;
                    if (lastM.at(wid - 1) < lastM.at(traceTemp))
                    {
                        traceTemp = wid - 1;
                    }
                    if (lastM.at(wid + 1) < lastM.at(traceTemp))
                    {
                        traceTemp = wid + 1;
                    }
                    curM.at(wid) = gradList.at(wid) + lastM.at(traceTemp);
                    traceLastIndex.at(wid) = traceTemp;
                }
                //wid == curW - 1
                {
                    if (lastM.at(curW - 2) < lastM.at(curW - 1))
                    {
                        curM.at(curW - 1) = gradList.at(curW - 1) + lastM.at(curW - 2);
                        traceLastIndex.at(curW - 1) = curW - 2;
                    }
                    else
                    {
                        curM.at(curW - 1) = gradList.at(curW - 1) + lastM.at(curW - 1);
                        traceLastIndex.at(curW - 1) = curW - 1;
                    }
                }
                //
                traceIndex.at(hid) = traceLastIndex;
                lastM = curM;
            }
            //chose the minimal cut index
            int minimalIndex = 0;
            for (int wid = 0; wid < curW; wid++)
            {
                if (curM.at(wid) < curM.at(minimalIndex))
                {
                    minimalIndex = wid;
                }
            }
            //move pixel and gradMat
            int cutPos = minimalIndex;
            for (int hid = 0; hid < originH; hid++)
            {
                for (int mid = cutPos; mid < curW - 1; mid++)
                {
                    unsigned char* pixel = img.ptr(hid, mid);
                    unsigned char* pixelNext = img.ptr(hid, mid + 1);
                    pixel[0] = pixelNext[0];
                    pixel[1] = pixelNext[1];
                    pixel[2] = pixelNext[2];
                    gradMat.at(hid).at(mid) = gradMat.at(hid).at(mid + 1);
                }
                cutPos = traceIndex.at(hid).at(cutPos);
            }
            //re-calculate cut grad
            cutPos = minimalIndex;
            for (int hid = 0; hid < originH; hid++)
            {
                if (cutPos < curW - 1)
                {
                    unsigned char* pixel = img.ptr(hid, cutPos);
                    unsigned char* pixelWNext = NULL;
                    if (cutPos == 0)
                    {
                        pixelWNext = img.ptr(hid, cutPos + 1);
                    }
                    else
                    {
                        pixelWNext = img.ptr(hid, cutPos - 1);
                    }
                    unsigned char* pixelHNext = NULL;
                    if (hid == 0)
                    {
                        pixelHNext = img.ptr(hid + 1, cutPos);
                    }
                    else
                    {
                        pixelHNext = img.ptr(hid - 1, cutPos);
                    }
                    gradMat.at(hid).at(cutPos) = abs(pixel[0] - pixelWNext[0]) + abs(pixel[1] - pixelWNext[1]) + abs(pixel[2] - pixelWNext[2]) +
                        abs(pixel[0] - pixelHNext[0]) + abs(pixel[1] - pixelHNext[1]) + abs(pixel[2] - pixelHNext[2]);
                }
                cutPos = traceIndex.at(hid).at(cutPos);
            }
        }
    }

    void Retargetting::SeamCarvingVertical(cv::Mat& img, int originW, int originH, int targetH)
    {

    }
}
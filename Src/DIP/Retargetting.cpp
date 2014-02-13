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
        int horizontalTime = inputW - targetW;
        if (horizontalTime > 0)
        {
            int verticalTime = inputH - targetH;
            if (verticalTime > 0)
            {
                float wScale = float(targetW) / inputW;
                float hScale = float(targetH) / inputH;
                if (wScale > hScale)
                {
                    cv::Mat imgPro = inputImg.clone();
                    int tempW = targetW;
                    int tempH = int(inputH * wScale);
                    cv::Size tempSize(tempW, tempH);
                    cv::Mat tempImg(tempSize, CV_8UC3);
                    cv::resize(imgPro, tempImg, tempSize);
                    SeamCarvingVertical(tempImg, tempW, tempH, targetH);
                    cv::Mat imgRes = tempImg.rowRange(0, targetH - 1);
                    return imgRes;
                }
                else
                {
                    cv::Mat imgPro = inputImg.clone();
                    int tempW = int(inputW * hScale);
                    int tempH = targetH;
                    cv::Size tempSize(tempW, tempH);
                    cv::Mat tempImg(tempSize, CV_8UC3);
                    cv::resize(imgPro, tempImg, tempSize);
                    SeamCarvingHorizontal(tempImg, tempW, tempH, targetW);
                    cv::Mat imgRes = tempImg.colRange(0, targetW - 1);
                    return imgRes;
                }
            }
            else
            {
                cv::Mat imgPro = inputImg.clone();
                int tempW = int(inputW * float(targetH) / inputH);
                int tempH = targetH;
                cv::Size tempSize(tempW, tempH);
                cv::Mat tempImg(tempSize, CV_8UC3);
                cv::resize(imgPro, tempImg, tempSize);
                SeamCarvingHorizontal(tempImg, tempW, tempH, targetW);
                cv::Mat imgRes = tempImg.colRange(0, targetW - 1);
                return imgRes;
            }
        }
        else
        {
            int verticalTime = inputH - targetH;
            if (verticalTime > 0)
            {
                cv::Mat imgPro = inputImg.clone();
                int tempW = targetW; 
                int tempH = int(inputH * float(targetW) / inputW);
                cv::Size tempSize(tempW, tempH);
                cv::Mat tempImg(tempSize, CV_8UC3);
                cv::resize(imgPro, tempImg, tempSize);
                SeamCarvingVertical(tempImg, tempW, tempH, targetH);
                cv::Mat imgRes = tempImg.rowRange(0, targetH - 1);
                return imgRes;
            }
            else
            {
                float wScale = float(targetW) / inputW;
                float HScale = float(targetH) / inputH;
                if (wScale > HScale)
                {
                    cv::Mat imgPro = inputImg.clone();
                    int tempW = targetW; 
                    int tempH = int(inputH * wScale);
                    cv::Size tempSize(tempW, tempH);
                    cv::Mat tempImg(tempSize, CV_8UC3);
                    cv::resize(imgPro, tempImg, tempSize);
                    SeamCarvingVertical(tempImg, tempW, tempH, targetH);
                    cv::Mat imgRes = tempImg.rowRange(0, targetH - 1);
                    return imgRes;
                }
                else
                {
                    cv::Mat imgPro = inputImg.clone();
                    int tempW = int(inputW * HScale);
                    int tempH = targetH;
                    cv::Size tempSize(tempW, tempH);
                    cv::Mat tempImg(tempSize, CV_8UC3);
                    cv::resize(imgPro, tempImg, tempSize);
                    SeamCarvingHorizontal(tempImg, tempW, tempH, targetW);
                    cv::Mat imgRes = tempImg.colRange(0, targetW - 1);
                    return imgRes;
                }
            }
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
            for (int hid = originH - 1; hid >= 0; hid--)
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
            for (int hid = originH - 1; hid >= 0; hid--)
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
        int cutTimes = originH - targetH;
        std::vector<std::vector<int> > gradMat(originW);
        for (int wid = 0; wid < originW; wid++)
        {
            std::vector<int> gradList(originH);
            for (int hid = 0; hid < originH; hid++)
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
                gradList.at(hid) = abs(pixel[0] - pixelWNext[0]) + abs(pixel[1] - pixelWNext[1]) + abs(pixel[2] - pixelWNext[2]) +
                    abs(pixel[0] - pixelHNext[0]) + abs(pixel[1] - pixelHNext[1]) + abs(pixel[2] - pixelHNext[2]);
            }
            gradMat.at(wid) = gradList;
        }
        std::vector<std::vector<int> > traceIndex(originW);
        std::vector<int> curM(originH);
        std::vector<int> lastM(originH, 0);
        std::vector<int> traceLastIndex(originH);
        for (int cutIndex = 0; cutIndex < cutTimes; cutIndex++)
        {
            int curH = originH - cutIndex;
            for (int wid = 0; wid < originW; wid++)
            {
                std::vector<int>& gradList = gradMat.at(wid);
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
                for (int hid = 1; hid < curH - 1; hid++)
                {
                    int traceTemp = hid;
                    if (lastM.at(hid - 1) < lastM.at(traceTemp))
                    {
                        traceTemp = hid - 1;
                    }
                    if (lastM.at(hid + 1) < lastM.at(traceTemp))
                    {
                        traceTemp = hid + 1;
                    }
                    curM.at(hid) = gradList.at(hid) + lastM.at(traceTemp);
                    traceLastIndex.at(hid) = traceTemp;
                }
                //wid == curW - 1
                {
                    if (lastM.at(curH - 2) < lastM.at(curH - 1))
                    {
                        curM.at(curH - 1) = gradList.at(curH - 1) + lastM.at(curH - 2);
                        traceLastIndex.at(curH - 1) = curH - 2;
                    }
                    else
                    {
                        curM.at(curH - 1) = gradList.at(curH - 1) + lastM.at(curH - 1);
                        traceLastIndex.at(curH - 1) = curH - 1;
                    }
                }
                //
                traceIndex.at(wid) = traceLastIndex;
                lastM = curM;
            }
            //chose the minimal cut index
            int minimalIndex = 0;
            for (int hid = 0; hid < curH; hid++)
            {
                if (curM.at(hid) < curM.at(minimalIndex))
                {
                    minimalIndex = hid;
                }
            }
            //move pixel and gradMat
            int cutPos = minimalIndex;
            //for (int wid = 0; wid < originW; wid++)
            for (int wid = originW - 1; wid >= 0; wid--)
            {
                for (int mid = cutPos; mid < curH - 1; mid++)
                {
                    unsigned char* pixel = img.ptr(mid, wid);
                    unsigned char* pixelNext = img.ptr(mid + 1, wid);
                    pixel[0] = pixelNext[0];
                    pixel[1] = pixelNext[1];
                    pixel[2] = pixelNext[2];
                    gradMat.at(wid).at(mid) = gradMat.at(wid).at(mid + 1);
                }
                cutPos = traceIndex.at(wid).at(cutPos);
            }
            //re-calculate cut grad
            cutPos = minimalIndex;
            //for (int wid = 0; wid < originW; wid++)
            for (int wid = originW - 1; wid >= 0; wid--)
            {
                if (cutPos < curH - 1)
                {
                    unsigned char* pixel = img.ptr(cutPos, wid);
                    unsigned char* pixelHNext = NULL;
                    if (cutPos == 0)
                    {
                        pixelHNext = img.ptr(cutPos + 1, wid);
                    }
                    else
                    {
                        pixelHNext = img.ptr(cutPos - 1, wid);
                    }
                    unsigned char* pixelWNext = NULL;
                    if (wid == 0)
                    {
                        pixelWNext = img.ptr(cutPos, wid + 1);
                    }
                    else
                    {
                        pixelWNext = img.ptr(cutPos, wid - 1);
                    }
                    gradMat.at(wid).at(cutPos) = abs(pixel[0] - pixelWNext[0]) + abs(pixel[1] - pixelWNext[1]) + abs(pixel[2] - pixelWNext[2]) +
                        abs(pixel[0] - pixelHNext[0]) + abs(pixel[1] - pixelHNext[1]) + abs(pixel[2] - pixelHNext[2]);
                }
                cutPos = traceIndex.at(wid).at(cutPos);
            }
        }
    }
}
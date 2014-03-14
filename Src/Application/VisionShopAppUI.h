#pragma once
#include "MyGUI.h"
#include "opencv2/opencv.hpp"

namespace MagicApp
{
    class VisionShopAppUI
    {
    public:
        VisionShopAppUI();
        ~VisionShopAppUI();

        void Setup();
        void Shutdown();

        void UpdateImageTexture(const cv::Mat& image);
        void UpdateMarkedImageTexture(const cv::Mat& image, const cv::Mat& mark);

    private:
        void OpenImage(MyGUI::Widget* pSender);
        void SaveImage(MyGUI::Widget* pSender);
        void ImageResizing(MyGUI::Widget* pSender);
        void SaliencyDetection(MyGUI::Widget* pSender);
        void DoImageResizing(MyGUI::Widget* pSender);
        void FastImageResizing(MyGUI::Widget* pSender);
        void SegmentImage(MyGUI::Widget* pSender);
        void BrushFront(MyGUI::Widget* pSender);
        void BrushBack(MyGUI::Widget* pSender);
        void SegmentImageDo(MyGUI::Widget* pSender);
        void BackHome(MyGUI::Widget* pSender);
        void Contact(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
        MyGUI::ITexture* mpImageTexture;
    };
}
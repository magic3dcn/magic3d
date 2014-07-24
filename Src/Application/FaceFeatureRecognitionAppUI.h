#pragma once
#include "MyGUI.h"
#include "opencv2/opencv.hpp"

namespace MagicApp
{
    class FaceFeatureRecognitionAppUI
    {
    public:
        FaceFeatureRecognitionAppUI();
        ~FaceFeatureRecognitionAppUI();

        void Setup();
        void Shutdown();
        void UpdateImage(const cv::Mat& image);

    private:
        void OpenImage(MyGUI::Widget* pSender);
        void LearnCascadedRegression(MyGUI::Widget* pSender);
        void ShapeRegression(MyGUI::Widget* pSender);
        void RealTimeFaceDetection(MyGUI::Widget* pSender);
        void BackHome(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
        MyGUI::ITexture* mpImageTexture;
    };

    
}

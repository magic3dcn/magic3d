#pragma once
#include "MyGUI.h"
#include "opencv2/opencv.hpp"

namespace MagicApp
{
    class FaceBeautificationAppUI
    {
    public:
        FaceBeautificationAppUI();
        ~FaceBeautificationAppUI();

        void Setup();
        void Shutdown();

        void UpdateLeftImage(const cv::Mat& image);
        void UpdateMiddleImage(const cv::Mat& image);
        void UpdateRightImage(const cv::Mat& image);
        void HideLeftImage(void);
        void HideMiddleImage(void);
        void HideRightImage(void);

    private:
        void UpdateImageTexture(MyGUI::ITexture** pImageTex, const cv::Mat& image, const std::string& texName);
        void OpenImage(MyGUI::Widget* pSender);
        void BackHome(MyGUI::Widget* pSender);
        void Contact(MyGUI::Widget* pSender);


    private:
        MyGUI::VectorWidgetPtr mRoot;
        MyGUI::ITexture* mpLeftImageTexture;
        MyGUI::ITexture* mpMiddleImageTexture;
        MyGUI::ITexture* mpRightImageTexture;
    };

}

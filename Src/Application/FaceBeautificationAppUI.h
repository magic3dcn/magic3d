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

        void Setup(int* maxFaceWidth, int* maxFaceHeight);
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
        void EditFeaturePoint(MyGUI::Widget* pSender);
        void DeformImageFeature(MyGUI::Widget* pSender);
        void DeformImageColor(MyGUI::Widget* pSender);
        void OpenReferenceImage(MyGUI::Widget* pSender);
        void CalReferenceImage(MyGUI::Widget* pSender);
        void DoFeaturePca(MyGUI::Widget* pSender);
        void DoColorPca(MyGUI::Widget* pSender);
        void DoMixPca(MyGUI::Widget* pSender);
        void CalMeanFace(MyGUI::Widget* pSender);
        void DeformFeatureToMeanFace(MyGUI::Widget* pSender);
        void DeformColorToMeanFace(MyGUI::Widget* pSender);
        void DeformToMeanFace(MyGUI::Widget* pSender);
        void BackHome(MyGUI::Widget* pSender);
        void Contact(MyGUI::Widget* pSender);


    private:
        MyGUI::VectorWidgetPtr mRoot;
        MyGUI::ITexture* mpLeftImageTexture;
        MyGUI::ITexture* mpMiddleImageTexture;
        MyGUI::ITexture* mpRightImageTexture;
    };

}

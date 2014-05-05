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
        void LoadFeaturePoint(MyGUI::Widget* pSender);
        void MoveFeaturePoint(MyGUI::Widget* pSender);
        void SaveFeaturePoint(MyGUI::Widget* pSender);
        void OpenReferenceImage(MyGUI::Widget* pSender);
        void LoadRefFeaturePoint(MyGUI::Widget* pSender);
        void ProjectRefFeaturePoint(MyGUI::Widget* pSender);
        void AlignFeaturePoint(MyGUI::Widget* pSender);
        void DeformOriginFace(MyGUI::Widget* pSender);
        void DoPca(MyGUI::Widget* pSender);
        void LoadPca(MyGUI::Widget* pSender);
        void ObservePca(MyGUI::Widget* pSender);
        void BackHome(MyGUI::Widget* pSender);
        void Contact(MyGUI::Widget* pSender);


    private:
        MyGUI::VectorWidgetPtr mRoot;
        MyGUI::ITexture* mpLeftImageTexture;
        MyGUI::ITexture* mpMiddleImageTexture;
        MyGUI::ITexture* mpRightImageTexture;
    };

}

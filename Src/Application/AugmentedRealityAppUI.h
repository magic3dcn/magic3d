#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class AugmentedRealityAppUI
    {
    public:
        AugmentedRealityAppUI();
        ~AugmentedRealityAppUI();

        void Setup();
        void Shutdown();

        void SetSliderPosition(int pos);
        void SetSliderRange(int range);
        void SetupImageBrowsing();

    private:
        void OpenVideo(MyGUI::Widget* pSender);
        void ChangeVideoFrame(MyGUI::ScrollBar* pSender, size_t pos);
        void CaptureFrame(MyGUI::Widget* pSender);
        void ClearCapture(MyGUI::Widget* pSender);
        void NextImage(MyGUI::Widget* pSender);
        void BackToHome(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };
}

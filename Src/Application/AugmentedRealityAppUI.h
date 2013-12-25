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

    private:
        void OpenVideo(MyGUI::Widget* pSender);
        void CameraCapture(MyGUI::Widget* pSender);
        void CameraRecord(MyGUI::Widget* pSender);
        void BackToHome(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };
}

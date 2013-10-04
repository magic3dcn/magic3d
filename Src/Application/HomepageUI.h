#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class HomepageUI
    {
    public:
        HomepageUI();
        ~HomepageUI();

        void Setup();
        void Shutdown();

    private:
        void EnterPointViewer(MyGUI::Widget* pSender);
        void EnterScan3D(MyGUI::Widget* pSender);
        void EnterReconstruction(MyGUI::Widget* pSender);
        void EnterLeapMotionApp(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };
}
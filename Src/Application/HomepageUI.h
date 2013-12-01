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
        void EnterPointShopApp(MyGUI::Widget* pSender);
        void EnterMeshShopApp(MyGUI::Widget* pSender);
        void EnterPointViewer(MyGUI::Widget* pSender);
        void EnterScan3D(MyGUI::Widget* pSender);
        void EnterReconstruction(MyGUI::Widget* pSender);
        void EnterReliefApp(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };
}
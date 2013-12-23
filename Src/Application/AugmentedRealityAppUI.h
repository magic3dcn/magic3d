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
        void BackToHome(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };
}

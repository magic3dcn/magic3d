#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class VisionShopAppUI
    {
    public:
        VisionShopAppUI();
        ~VisionShopAppUI();

        void Setup();
        void Shutdown();

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };
}
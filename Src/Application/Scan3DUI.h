#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class Scan3DUI
    {
    public:
        Scan3DUI();
        ~Scan3DUI();

        void Setup();
        void Shutdown();

    private:
        void Scan(MyGUI::Widget* pSender);
        void BackToHomepage(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };

}
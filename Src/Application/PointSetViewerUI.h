#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class PointSetViewerUI
    {
    public:
        PointSetViewerUI();
        ~PointSetViewerUI();

        void Setup();
        void Shutdown();

    private:
        void OpenPointSet(MyGUI::Widget* pSender);
        void BackToHomepage(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };
}
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
        void Record(MyGUI::Widget* pSender);
        void PlayScanner(MyGUI::Widget* pSender);
        void PlayRecordScanner(MyGUI::Widget* pSender);
        void CapturePointSet(MyGUI::Widget* pSender);
        void BackToHomepage(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };

}
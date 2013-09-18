#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class ScanningAppUI
    {
    public:
        ScanningAppUI();
        ~ScanningAppUI();

        void Setup();
        void Shutdown();

    private:
        void RecordDepth(MyGUI::Widget* pSender);
        void BackHome(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
        bool mIsRecording;
    };
}
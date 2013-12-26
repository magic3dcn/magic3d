#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class VideoRecordingAppUI
    {
    public:
        VideoRecordingAppUI();
        ~VideoRecordingAppUI();

        void Setup();
        void Shutdown();

    private:
        void BackToHome(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };

}
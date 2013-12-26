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
        void Record(MyGUI::Widget* pSender);
        void BackToHome(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
        bool mIsRecording;
    };

}
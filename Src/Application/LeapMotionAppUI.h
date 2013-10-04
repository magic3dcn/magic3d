#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class LeapMotionAppUI
    {
    public:
        LeapMotionAppUI();
        ~LeapMotionAppUI();

        void Setup();
        void Shutdown();

    private:
        void OpenMesh3D(MyGUI::Widget* pSender);
        void BackToHome(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };


}
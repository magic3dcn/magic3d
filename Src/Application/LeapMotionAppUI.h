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
        MyGUI::VectorWidgetPtr mRoot;
    };


}
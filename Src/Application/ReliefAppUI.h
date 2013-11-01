#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class ReliefAppUI
    {
    public:
        ReliefAppUI();
        ~ReliefAppUI();

        void Setup();
        void Shutdown();

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };
}
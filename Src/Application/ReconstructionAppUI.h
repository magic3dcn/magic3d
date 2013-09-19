#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class ReconstructionAppUI
    {
    public:
        ReconstructionAppUI();
        ~ReconstructionAppUI();

        void Setup();
        void Shutdown();

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };

}
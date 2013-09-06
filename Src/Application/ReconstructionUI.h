#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class ReconstructionUI
    {
    public:
        ReconstructionUI();
        ~ReconstructionUI();

        void Setup();
        void Shutdown();

    private:
        void OpenPointSet(MyGUI::Widget* pSender);
        void FilterPointSet(MyGUI::Widget* pSender);
        void AlignPointSet(MyGUI::Widget* pSender);
        void TSDFExtration(MyGUI::Widget* pSender);
        void BackToHomepage(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };
}
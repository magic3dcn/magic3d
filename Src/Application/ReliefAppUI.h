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
        void OpenPointCloud(MyGUI::Widget* pSender);
        void GenerateRelief(MyGUI::Widget* pSender);
        void BackHome(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };
}
#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class PrimitiveDetectionAppUI
    {
    public:
        PrimitiveDetectionAppUI();
        ~PrimitiveDetectionAppUI();

        void Setup();
        void Shutdown();

    private:
        void OpenMesh3D(MyGUI::Widget* pSender);
        void RansacMethod(MyGUI::Widget* pSender);
        void BackToHome(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };

}
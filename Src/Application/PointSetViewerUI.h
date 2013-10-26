#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class PointSetViewerUI
    {
    public:
        PointSetViewerUI();
        ~PointSetViewerUI();

        void Setup();
        void Shutdown();

    private:
        void OpenPointSet(MyGUI::Widget* pSender);
        void SavePointSet(MyGUI::Widget* pSender);
        void ReconstrutctPointSet(MyGUI::Widget* pSender);
        void OpenMesh3D(MyGUI::Widget* pSender);
        void SaveMesh3D(MyGUI::Widget* pSender);
        void FilterMesh3D(MyGUI::Widget* pSender);
        void BackToHomepage(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };
}
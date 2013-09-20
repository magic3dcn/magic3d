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
        void SetProgressBarPosition(int pos);
        void SetProgressBarRange(int range);

    private:
        void SetupRecordPlayback();
        void SetupReconstructProgress();
        void SetupReconstructing();
        void SetupMeshProcessing();

        void OpenScanRecord(MyGUI::Widget* pSender);
        void BackHome(MyGUI::Widget* pSender);
        void SetTimeRange(MyGUI::Widget* pSender);
        void SetLeftRange(MyGUI::Widget* pSender, int rel);
        void SetRightRange(MyGUI::Widget* pSender, int rel);
        void SetTopRange(MyGUI::Widget* pSender, int rel);
        void SetDownRange(MyGUI::Widget* pSender, int rel);
        void SetFrontRange(MyGUI::Widget* pSender, int rel);
        void SetBackRange(MyGUI::Widget* pSender, int rel);
        void SavePointSet(MyGUI::Widget* pSender);
        void Reconstruction(MyGUI::Widget* pSender);
        void SaveMesh3D(MyGUI::Widget* pSender);
        void SmoothMesh3D(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };

}
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
        void StartPostProcess();

    private:
        void SetupRecordPlayback();
        void SetupReconstructProgress();
        void SetupReconstructing();
        void SetupMeshProcessing();

        void OpenScanRecord(MyGUI::Widget* pSender);
        void BackHome(MyGUI::Widget* pSender);
        void ChangeFrameStart(MyGUI::ScrollBar* pSender, size_t pos);
        void ChangeFrameEnd(MyGUI::ScrollBar* pSender, size_t pos);
        void PointSetAlign(MyGUI::Widget* pSender);
        void ChangeLeftRange(MyGUI::Widget* pSender, int rel);
        void ChangeRightRange(MyGUI::Widget* pSender, int rel);
        void ChangeTopRange(MyGUI::Widget* pSender, int rel);
        void ChangeDownRange(MyGUI::Widget* pSender, int rel);
        void ChangeFrontRange(MyGUI::Widget* pSender, int rel);
        void ChangeBackRange(MyGUI::Widget* pSender, int rel);
        void SavePointSet(MyGUI::Widget* pSender);
        void Reconstruction(MyGUI::Widget* pSender);
        void RemovePointSetOutlier(MyGUI::Widget* pSender);
        void SaveMesh3D(MyGUI::Widget* pSender);
        void SmoothMesh3D(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
        int mFrameStartIndex;
        int mFrameEndIndex;
    };

}
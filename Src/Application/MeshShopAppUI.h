#pragma once
#include "MyGUI.h"

namespace MagicApp
{
    class MeshShopAppUI
    {
    public:
        MeshShopAppUI();
        ~MeshShopAppUI();

        void Setup();
        void Shutdown();
        
        void SetupFromMeshInput(int vertNum);

    private:
        void OpenMesh(MyGUI::Widget* pSender);
        void SaveMesh(MyGUI::Widget* pSender);
        void FilterMesh(MyGUI::Widget* pSender);
        void SmoothMesh(MyGUI::Widget* pSender);
        void RemoveOutlier(MyGUI::Widget* pSender);
        void AddNoise(MyGUI::Widget* pSender);
        void ViewMesh(MyGUI::Widget* pSender);
        void SelectMesh(MyGUI::Widget* pSender);
        void RectangleSelect(MyGUI::Widget* pSender);
        void CycleSelect(MyGUI::Widget* pSender);
        void ClearSelect(MyGUI::Widget* pSender);
        void DeleteSelect(MyGUI::Widget* pSender);
        void BackToHome(MyGUI::Widget* pSender);
        void Contact(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
    };


}
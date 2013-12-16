#pragma once
#include "../Common/AppBase.h"
#include "MeshShopAppUI.h"
#include "../Tool/ViewTool.h"
#include "../DGP/Mesh3D.h"
#include "../Tool/PickPointTool.h"

namespace MagicApp
{
    class MeshShopApp : public MagicCore::AppBase
    {
    public:
        enum MouseMode
        {
            MM_None = 0,
            MM_View,
            MM_Pick_Rectangle,
            MM_Pick_Cycle
        };

        MeshShopApp();
        ~MeshShopApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );

        bool OpenMesh(int& vertNum);
        void SaveMesh();
        void SmoothMesh();
        void RemoveOutlier();
        void AddNoise();
        void RectangleSelect();
        void CycleSelect();
        void ClearSelect();
        void DeleteSelcetVertex();
        void ModelViewer();

        void SetupFromMeshInput(MagicDGP::Mesh3D* pMesh);

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void UpdateMeshRendering();
        void ClearSceneData(void);

    private:
        MeshShopAppUI mUI;
        MagicTool::ViewTool mViewTool;
        MagicDGP::Mesh3D* mpMesh;
        MagicDGP::LightMesh3D* mpLightMesh;
        MouseMode mMouseMode;
        MagicTool::PickPointTool mPickTool;
        MagicDGP::Vector3 mDefaultColor;
        std::set<int> mPickIndexSet;
    };

}
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
        void SetPickIgnoreBack(bool ignore);
        void ModelViewer();

        void SetupFromMeshInput(MagicDGP::LightMesh3D* pMesh);

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void UpdateMeshRendering();
        void ClearSceneData(void);

        void ExtractDepthDataTest();

    private:
        MeshShopAppUI mUI;
        MagicTool::ViewTool mViewTool;
        MagicDGP::LightMesh3D* mpLightMesh;
        MouseMode mMouseMode;
        bool mPickIgnoreBack;
        MagicTool::PickPointTool mPickTool;
        MagicMath::Vector3 mDefaultColor;
        std::set<int> mPickIndexSet;
    };

}
#pragma once
#include "../Common/AppBase.h"
#include "MeshShopAppUI.h"
#include "../Tool/ViewTool.h"
#include "../DGP/Mesh3D.h"

namespace MagicApp
{
    class MeshShopApp : public MagicCore::AppBase
    {
    public:
        MeshShopApp();
        ~MeshShopApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );

        bool OpenMesh();
        void SaveMesh();
        void SmoothMesh();
        void SimplifyMesh();
        void RemoveOutlier();
        void AddNoise();
        void RectangleSelect();
        void CycleSelect();
        void IntelligentSelect();
        void DeformMesh();

    private:
        void SetupScene(void);
        void ShutdownScene(void);

    private:
        MeshShopAppUI mUI;
        MagicTool::ViewTool mViewTool;
        MagicDGP::Mesh3D* mpMesh;
    };

}
#pragma once
#include "../Common/AppBase.h"
#include "ReliefAppUI.h"
#include "../Tool/ViewTool.h"
#include "../DGP/PointCloud3D.h"
#include "../DGP/Mesh3D.h"

namespace MagicApp
{
    class ReliefApp : public MagicCore::AppBase
    {
    public:
        ReliefApp();
        ~ReliefApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

        bool ImportPointSet();
        void GenerateRelief();

    private:
        void SetupScene(void);
        void ShutdownScene(void);

    private:
        ReliefAppUI mUI;
        MagicTool::ViewTool mViewTool;
        MagicDGP::Point3DSet* mpPointSet;
        MagicDGP::Mesh3D* mpMesh;
    };
}
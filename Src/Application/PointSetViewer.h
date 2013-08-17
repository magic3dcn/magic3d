#pragma once
#include "../Common/AppBase.h"
#include "../Tool/ViewTool.h"
#include "../DGP/PointCloud3D.h"
#include "PointSetViewerUI.h"

namespace MagicApp
{
    class PointSetViewer : public MagicCore::AppBase
    {
    public: 
        PointSetViewer();
        virtual ~PointSetViewer();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

        void SetPointSet(MagicDGP::Point3DSet* pPointSet);

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        
    private:
        PointSetViewerUI mUI;
        MagicTool::ViewTool mViewTool;
        MagicDGP::Point3DSet* mpPointSet;
    };
}
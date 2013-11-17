#pragma once
#include "../Common/AppBase.h"
#include "../Tool/ViewTool.h"
#include "../DGP/PointCloud3D.h"
#include "../DGP/Mesh3D.h"
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
        virtual bool KeyPressed( const OIS::KeyEvent &arg );

        bool ImportPointSet();
        void ExportPointSet();
        void FilterPointSetOutliers();
        void SamplePointSet();
        bool ReconstructPointSet();
        bool ImportMesh3D();
        void ExportMesh3D();
        bool FilterMesh3D();

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        
    private:
        PointSetViewerUI mUI;
        MagicTool::ViewTool mViewTool;
        MagicDGP::Point3DSet* mpPointSet;
        MagicDGP::Mesh3D* mpMesh;
    };
}
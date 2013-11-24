#pragma once
#include "../Common/AppBase.h"
#include "../Tool/ViewTool.h"
#include "PrimitiveDetectionAppUI.h"
#include "../DGP/Mesh3D.h"
#include "../DGP/PrimitiveDetection.h"

namespace MagicApp
{
    class PrimitiveDetectionApp : public MagicCore::AppBase
    {
    public:
        PrimitiveDetectionApp();
        virtual ~PrimitiveDetectionApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );

        bool ImportMesh3D();
        void RansacPrimitiveDetection();
        void PrimitiveSelection(int sampleId);
        void CalMeshCurvature();
        void FilterMesh3D();
        void CalNormalDeviation();
        void SampleVertex();
        void DrawPrimitive(MagicDGP::ShapeCandidate* pCand);

    private:
        void SetupScene(void);
        void ShutdownScene(void);

    private:
        PrimitiveDetectionAppUI mUI;
        MagicTool::ViewTool mViewTool;
        MagicDGP::Mesh3D* mpMesh;
        bool mIsPickingMode;
    };

}

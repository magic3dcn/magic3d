#pragma once
#include "../Common/AppBase.h"
#include "../Tool/ViewTool.h"
#include "../DGP/PointCloud3D.h"
#include "ReconstructionUI.h"
#include <map>
#include <string>
#include "../DGP/SignedDistanceFunction.h"

namespace MagicApp
{
    class Reconstruction : public MagicCore::AppBase
    {
    public: 
        Reconstruction();
        virtual ~Reconstruction();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

        std::string AddPoint3DSet(MagicDGP::Point3DSet* pPS);
        void UpdatePCRendering();
        void FilterPointSet();
        void AlignPointSet();
        void TSDFExtraction();
        void PointSetFusion();
        void Clear();

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        
    private:
        ReconstructionUI mUI;
        MagicTool::ViewTool mViewTool;
        std::map<std::string, MagicDGP::Point3DSet* > mPCSet;
        MagicDGP::SignedDistanceFunction mSdf;
    };
}
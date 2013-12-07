#pragma once
#include "../Common/AppBase.h"
#include "PointShopAppUI.h"
#include "../Tool/ViewTool.h"
#include "../DGP/PointCloud3D.h"

namespace MagicApp
{
    class PointShopApp : public MagicCore::AppBase
    {
    public:
        PointShopApp();
        ~PointShopApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );

        bool OpenPointSet(bool& hasNormal, int& pointNum);
        void SavePointSet();
        void CalPointSetNormal();
        void FlipPointSetNormal();
        void SmoothPointSet();
        bool SamplePointSet(int sampleNum);
        void RemoveOutlier();
        void Reconstruction();
        void AddNoise();
        void RectangleSelect();
        void CycleSelect();
        void IntelligentSelect();
        void DeformPointSet();

        void SetupFromPointsetInput(MagicDGP::Point3DSet* pPS);

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void UpdatePointSetRendering();

    private:
        PointShopAppUI mUI;
        MagicTool::ViewTool mViewTool;
        MagicDGP::Point3DSet* mpPointSet;
    };
}
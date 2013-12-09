#pragma once
#include "../Common/AppBase.h"
#include "PointShopAppUI.h"
#include "../Tool/ViewTool.h"
#include "../Tool/PickPointTool.h"
#include "../DGP/PointCloud3D.h"

namespace MagicApp
{
    class PointShopApp : public MagicCore::AppBase
    {
    public:
        enum MouseMode
        {
            MM_None = 0,
            MM_View,
            MM_Pick
        };

        PointShopApp();
        ~PointShopApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
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

        void SetupFromPointsetInput(MagicDGP::Point3DSet* pPS);

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void UpdatePointSetRendering();

    private:
        PointShopAppUI mUI;
        MagicDGP::Point3DSet* mpPointSet;
        MagicTool::ViewTool mViewTool;
        MagicTool::PickPointTool mPickTool;
        MouseMode mMouseMode;
    };
}
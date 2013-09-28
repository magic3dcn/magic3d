#pragma once
#include "../Common/AppBase.h"
#include "ReconstructionAppUI.h"
#include "OpenNI.h"
#include "../DGP/PointCloud3D.h"
#include "../DGP/Mesh3D.h"
#include "../Tool/ViewTool.h"

namespace MagicApp
{
    class ReconstructionApp  : public MagicCore::AppBase
    {
    public:
        ReconstructionApp();
        ~ReconstructionApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    public:
        bool OpenSceneRecord();
        void SetTimeStart();
        void SetTimeEnd();
        void ChangeLeftRange(int rel);
        void ChangeRightRange(int rel);
        void ChangeTopRange(int rel);
        void ChangeDownRange(int rel);
        void ChangeFrontRange(int rel);
        void ChangeBackRange(int rel);
        void PointSetRegistration();
        void SetupPointSetProcessing();
        bool SavePointSet();
        void SmoothPointSet();
        bool ReconstructPointSet();
        bool SaveMesh3D();
        void SmoothMesh3D();
       
    private:
        void SetupRenderScene();
        void ReleaseRenderScene(void);
        bool SetupDevice();
        void ReleaseDevice();
        void UpdateScannerDisplay();
        void CoarseRangeLimitCalculation(const std::vector<MagicDGP::Vector3>& posList);
        MagicDGP::Point3DSet* GetPointSetFromRecord(int frameId);

    private:
        ReconstructionAppUI mUI;
        MagicTool::ViewTool mViewTool;
        bool mUsingViewTool;
        openni::Device mDevice;
        openni::VideoStream mDepthStream;
        bool mIsScannerDisplaying;
        MagicDGP::Point3DSet* mpPointSet;
        MagicDGP::Mesh3D* mpMesh;
        float mLeftLimit, mRightLimit, mTopLimit, mDownLimit, mFrontLimit, mBackLimit;
        int mFrameStartIndex, mFrameEndIndex, mFrameCurrent;
        bool mIsSetFrameStart, mIsSetFrameEnd;
        bool mIsNeedRangeLimitCaculation;
    };

}
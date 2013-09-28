#pragma once
#include "../Common/AppBase.h"
#include "ScanningAppUI.h"
#include "OpenNI.h"

namespace MagicApp
{
    class ScanningApp : public MagicCore::AppBase
    {
    public:
        ScanningApp();
        ~ScanningApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);

        void StartRecord();
        void StopRecord();

    private:
        bool SetupDevice();
        void SetupRenderScene();
        void ReleaseDevice();
        void ReleaseRenderScene();
        void UpdateScannerDisplay();

    private:
        bool mIsDeviceSetup;
        ScanningAppUI mUI;
        openni::Device mDevice;
        openni::VideoStream mDepthStream, mColorStream;
        openni::Recorder mRecorder;
    };
}
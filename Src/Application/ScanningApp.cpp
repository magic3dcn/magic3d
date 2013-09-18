#include "ScanningApp.h"
#include "../Common/LogSystem.h"

namespace MagicApp
{
    ScanningApp::ScanningApp()
    {
    }

    ScanningApp::~ScanningApp()
    {
    }

    bool ScanningApp::Enter(void)
    {
        MagicLog << "Enter ScanningApp" << std::endl;
        mUI.Setup();
        return true;
    }

    bool ScanningApp::Update(float timeElapsed)
    {
        return true;
    }

    bool ScanningApp::Exit(void)
    {
        mUI.Shutdown();
        return true;
    }
}
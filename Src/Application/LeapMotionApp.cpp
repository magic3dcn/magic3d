#include "LeapMotionApp.h"

namespace MagicApp
{
    LeapMotionApp::LeapMotionApp()
    {
    }

    LeapMotionApp::~LeapMotionApp()
    {
    }

    bool LeapMotionApp::Enter(void)
    {
        mUI.Setup();

        return true;
    }

    bool LeapMotionApp::Update(float timeElapsed)
    {
        return true;
    }

    bool LeapMotionApp::Exit(void)
    {
        mUI.Shutdown();

        return true;
    }
}
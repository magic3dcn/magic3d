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

    void LeapMotionApp::onInit(const Leap::Controller& controller)
    {

    }

    void LeapMotionApp::onConnect(const Leap::Controller& controller)
    {

    }

    void LeapMotionApp::onDisconnect(const Leap::Controller& controller)
    {

    }

    void LeapMotionApp::onExit(const Leap::Controller& controller)
    {

    }

    void LeapMotionApp::onFrame(const Leap::Controller& controller)
    {

    }

    void LeapMotionApp::onFocusGained(const Leap::Controller& controller)
    {

    }

    void LeapMotionApp::onFocusLost(const Leap::Controller& controller)
    {

    }
}
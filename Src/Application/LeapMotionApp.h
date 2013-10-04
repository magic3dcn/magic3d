#pragma once
#include "Leap.h"
#include "../Common/AppBase.h"
#include "LeapMotionAppUI.h"

namespace MagicApp
{
    class LeapMotionApp : public MagicCore::AppBase, public Leap::Listener
    {
    public:
        LeapMotionApp();
        virtual ~LeapMotionApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);

        virtual void onInit(const Leap::Controller& controller);
        virtual void onConnect(const Leap::Controller& controller);
        virtual void onDisconnect(const Leap::Controller& controller);
        virtual void onExit(const Leap::Controller& controller);
        virtual void onFrame(const Leap::Controller& controller);
        virtual void onFocusGained(const Leap::Controller& controller);
        virtual void onFocusLost(const Leap::Controller& controller);

    private:
        LeapMotionAppUI mUI;
        Leap::Controller mLeapCtrl;
    };


}
#pragma once
#include "../Common/AppBase.h"
#include "LeapMotionAppUI.h"

namespace MagicApp
{
    class LeapMotionApp : public MagicCore::AppBase
    {
    public:
        LeapMotionApp();
        virtual ~LeapMotionApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);

    private:
        LeapMotionAppUI mUI;
    };


}
#pragma once
#include "../Common/AppBase.h"

namespace MagicApp
{
    class PointSetViewer : public MagicCore::AppBase
    {
    public: 
        PointSetViewer();
        virtual ~PointSetViewer();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool Pause(void);
        virtual bool Resume(void);

    };
}
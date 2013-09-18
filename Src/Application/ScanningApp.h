#pragma once
#include "../Common/AppBase.h"
#include "ScanningAppUI.h"

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

    private:
        ScanningAppUI mUI;
    };
}
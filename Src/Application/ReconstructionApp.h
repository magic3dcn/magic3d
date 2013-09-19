#pragma once
#include "../Common/AppBase.h"
#include "ReconstructionAppUI.h"

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

    private:
        ReconstructionAppUI mUI;
    };

}
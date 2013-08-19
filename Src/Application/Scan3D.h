#pragma once
#include "../Common/AppBase.h"
#include "Scan3DUI.h"

namespace MagicApp
{
    class Scan3D : public MagicCore::AppBase
    {
    public:
        Scan3D();
        ~Scan3D();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);

    private:
        Scan3DUI mUI;
    };


}
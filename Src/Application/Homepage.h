#pragma once
#include "../Common/AppBase.h"
#include "HomepageUI.h"

namespace MagicApp
{
    class Homepage : public MagicCore::AppBase
    {
    public: 
        Homepage();
        virtual ~Homepage();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);

        virtual bool KeyPressed( const OIS::KeyEvent &arg );

    private:
        HomepageUI mUI;
    };
}
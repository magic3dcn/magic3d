#pragma once
#include "../Common/AppBase.h"
#include "PointShopAppUI.h"

namespace MagicApp
{
    class PointShopApp : public MagicCore::AppBase
    {
    public:
        PointShopApp();
        ~PointShopApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );

    private:
        void SetupScene(void);
        void ShutdownScene(void);

    private:
        PointShopAppUI mUI;
    };
}
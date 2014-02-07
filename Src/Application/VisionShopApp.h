#pragma once
#include "../Common/AppBase.h"
#include "VisionShopAppUI.h"

namespace MagicApp
{
    class VisionShopApp : public MagicCore::AppBase
    {
    public:
        VisionShopApp();
        ~VisionShopApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );

    private:
        void SetupScene(void);
        void ShutdownScene(void);

    private:
        VisionShopAppUI mUI;
    };
}

#pragma once
#include "../Common/AppBase.h"
#include "FaceBeautificationAppUI.h"

namespace MagicApp
{
    class FaceBeautificationApp : public MagicCore::AppBase
    {
    public:
        FaceBeautificationApp();
        ~FaceBeautificationApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );
        virtual void WindowResized( Ogre::RenderWindow* rw );

        bool OpenImage(void);

    private:
        void SetupScene(void);
        void ShutdownScene(void);

    private:
        FaceBeautificationAppUI mUI;
        cv::Mat mImage;
    };

    

}

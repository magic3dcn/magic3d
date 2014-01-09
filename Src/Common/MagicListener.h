#pragma once
#include "OIS.h"
#include "OgreFrameListener.h"
#include "OgreWindowEventUtilities.h"
//#include "../Common/MagicOgre.h"

namespace Ogre
{
    //class FrameListener;
    //class WindowEventListener;
    class FrameEvent;
    class RenderWindow;
}

namespace MagicCore
{
    class MagicListener : 
        public OIS::MouseListener, public OIS::KeyListener, 
        public Ogre::FrameListener, public Ogre::WindowEventListener
    {
    private:
        static MagicListener* mpListener;
        MagicListener();

    public:
        static MagicListener* GetSingleton();

        virtual bool frameStarted(const Ogre::FrameEvent& evt);
        virtual bool frameEnded(const Ogre::FrameEvent& evt);
        virtual bool mouseMoved( const OIS::MouseEvent &arg );
        virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool keyPressed( const OIS::KeyEvent &arg );
        virtual bool keyReleased( const OIS::KeyEvent &arg );

        virtual void windowResized(Ogre::RenderWindow* rw);
        virtual bool windowClosing(Ogre::RenderWindow* rw);
        virtual void windowFocusChange(Ogre::RenderWindow* rw);

        virtual ~MagicListener();
    };
}
#include "StdAfx.h"
#include "MagicListener.h"
#include "AppManager.h"

namespace MagicCore
{
    MagicListener* MagicListener::mpListener = NULL;

    MagicListener::MagicListener()
    {
    }

    MagicListener* MagicListener::GetSingleton()
    {
	    if (mpListener == NULL)
	    {
		    mpListener = new MagicListener;
	    }
	    return mpListener;
    }

    bool MagicListener::frameStarted(const Ogre::FrameEvent& evt)
    {
        MagicCore::FrameEvent fe = {evt.timeSinceLastEvent, evt.timeSinceLastFrame};
        return AppManager::GetSingleton()->FrameStarted(fe);
    }

    bool MagicListener::frameEnded(const Ogre::FrameEvent& evt)
    {
        MagicCore::FrameEvent fe = {evt.timeSinceLastEvent, evt.timeSinceLastFrame};
        return AppManager::GetSingleton()->FrameEnded(fe);
    }

    bool MagicListener::mouseMoved( const OIS::MouseEvent &arg )
    {
        CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
		CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseWheelChange(arg.state.Z.rel / 120.0f);
        return AppManager::GetSingleton()->MouseMoved(arg);
    }

    bool MagicListener::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertOISButtonToCegui(id));
        return AppManager::GetSingleton()->MousePressed(arg, id);
    }

    bool MagicListener::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertOISButtonToCegui(id));
        return AppManager::GetSingleton()->MouseReleased(arg, id);
    }

    bool MagicListener::keyPressed( const OIS::KeyEvent &arg )
    {
      //  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(arg.key);
        return AppManager::GetSingleton()->KeyPressed(arg);
    }

    bool MagicListener::keyReleased( const OIS::KeyEvent &arg )
    {
      //  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(arg.key);
        return AppManager::GetSingleton()->KeyReleased(arg);
    }

    CEGUI::MouseButton MagicListener::convertOISButtonToCegui(int buttonID)
    {
        switch (buttonID)
        {
        case OIS::MB_Left:
            return CEGUI::LeftButton;
        case OIS::MB_Right:
            return CEGUI::RightButton;
        case OIS::MB_Middle:
            return CEGUI::MiddleButton;
        default:
            return CEGUI::LeftButton;
        }
    }


    MagicListener::~MagicListener()
    {
    }
}
//#include "StdAfx.h"
#include "MagicListener.h"
#include "AppManager.h"
#include "MyGUI.h"
#include "ToolKit.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "GUISystem.h"

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
        ToolKit::GetSingleton()->SetMousePressLocked(false);
        MyGUI::InputManager::getInstance().injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
        return AppManager::GetSingleton()->MouseMoved(arg);
        
    }

    bool MagicListener::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (ToolKit::GetSingleton()->IsMousePressLocked())
        {
            return true;
        }
        else
        {
            MyGUI::InputManager::getInstance().injectMousePress(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
            return AppManager::GetSingleton()->MousePressed(arg, id);
        }
    }

    bool MagicListener::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (ToolKit::GetSingleton()->IsMousePressLocked())
        {
            return true;
        }
        else
        {
            MyGUI::InputManager::getInstance().injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
            return AppManager::GetSingleton()->MouseReleased(arg, id);
        }
    }

    bool MagicListener::keyPressed( const OIS::KeyEvent &arg )
    {
        MyGUI::KeyCode code = MyGUI::KeyCode::Enum(arg.key);
        MyGUI::InputManager::getInstance().injectKeyPress(code, arg.text);
        return AppManager::GetSingleton()->KeyPressed(arg);
    }

    bool MagicListener::keyReleased( const OIS::KeyEvent &arg )
    {
        MyGUI::KeyCode code = MyGUI::KeyCode::Enum(arg.key);
        MyGUI::InputManager::getInstance().injectKeyRelease(code);
        return AppManager::GetSingleton()->KeyReleased(arg);
    }

    void MagicListener::windowResized(Ogre::RenderWindow* rw)
    {
        RenderSystem::GetSingleton()->GetMainCamera()->setAspectRatio((Ogre::Real)rw->getWidth() / (Ogre::Real)rw->getHeight());
        InputSystem::GetSingleton()->UpdateMouseState(rw->getWidth(), rw->getHeight());
        AppManager::GetSingleton()->WindowResized(rw);
    }

    bool MagicListener::windowClosing(Ogre::RenderWindow* rw)
    {
        ToolKit::GetSingleton()->SetAppRunning(false);
        return true;
    }

    void MagicListener::windowFocusChange(Ogre::RenderWindow* rw)
    {
        ToolKit::GetSingleton()->SetMousePressLocked(true);
    }

    MagicListener::~MagicListener()
    {
    }
}

//#include "StdAfx.h"
#include "MagicListener.h"
#include "AppManager.h"
#include "MyGUI.h"
#include "ToolKit.h"
#include "RenderSystem.h"
#include "InputSystem.h"

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
        MyGUI::InputManager::getInstance().injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
        return AppManager::GetSingleton()->MouseMoved(arg);
    }

    bool MagicListener::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        MyGUI::InputManager::getInstance().injectMousePress(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
        return AppManager::GetSingleton()->MousePressed(arg, id);
    }

    bool MagicListener::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        MyGUI::InputManager::getInstance().injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
        return AppManager::GetSingleton()->MouseReleased(arg, id);
    }

    bool MagicListener::keyPressed( const OIS::KeyEvent &arg )
    {
        MyGUI::InputManager::getInstance().injectKeyPress(MyGUI::KeyCode::Enum(arg.key));
        return AppManager::GetSingleton()->KeyPressed(arg);
    }

    bool MagicListener::keyReleased( const OIS::KeyEvent &arg )
    {
        MyGUI::InputManager::getInstance().injectKeyRelease(MyGUI::KeyCode::Enum(arg.key));
        return AppManager::GetSingleton()->KeyReleased(arg);
    }

    void MagicListener::windowResized(Ogre::RenderWindow* rw)
    {
        rw->windowMovedOrResized();
        RenderSystem::GetSingleton()->GetMainCamera()->setAspectRatio((Ogre::Real)rw->getWidth() / (Ogre::Real)rw->getHeight());
        InputSystem::GetSingleton()->UpdateMouseState(rw->getWidth(), rw->getHeight());
    }

    bool MagicListener::windowClosing(Ogre::RenderWindow* rw)
    {
        ToolKit::GetSingleton()->SetAppRunning(false);
        return true;
    }

    MagicListener::~MagicListener()
    {
    }
}

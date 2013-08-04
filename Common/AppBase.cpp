#include "StdAfx.h"
#include "AppBase.h"

namespace MagicCore
{
    AppBase::AppBase(void)
    {
    }

    bool AppBase::Enter()
    {
        return true;
    }

    bool AppBase::Update(float timeElapsed)
    {
        return true;
    }

    bool AppBase::Exit()
    {
        return true;
    }

    bool AppBase::Pause()
    {
        return true;
    }

    bool AppBase::Resume()
    {
        return true;
    }

    bool AppBase::FrameStarted(const FrameEvent& evt)
    {
        return true;
    }

    bool AppBase::FrameEnded(const FrameEvent& evt)
    {
        return true;
    }

    bool AppBase::MouseMoved( const OIS::MouseEvent &arg )
    {
        return true;
    }

    bool AppBase::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool AppBase::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool AppBase::KeyPressed( const OIS::KeyEvent &arg )
    {
        return true;
    }

    bool AppBase::KeyReleased( const OIS::KeyEvent &arg )
    {
        return true;
    }
 
    AppBase::~AppBase(void)
    {
    }
}

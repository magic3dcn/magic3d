#include "StdAfx.h"
#include "AppManager.h"
#include "ToolKit.h"
#include "LogSystem.h"
#include "../Application/Homepage.h"
#include "../Application/PointSetViewer.h"

namespace MagicCore
{
    AppManager* AppManager::mpAppMgr = NULL;

    AppManager::AppManager(void)
    {
    }

    AppManager* AppManager::GetSingleton()
    {
        if (mpAppMgr == NULL)
        {
            mpAppMgr = new AppManager;
        }
        return mpAppMgr;
    }

    void AppManager::Init()
    {
        PushApp(new MagicApp::PointSetViewer);
    }

    void AppManager::Update(float timeElapsed)
    {
        //MagicLog << "    AppManager::Update: " << timeElapsed << std::endl;
        if (!mAppList.empty())
        {
            mAppList.back()->Update(timeElapsed);
        }
    }

    void AppManager::PushApp(AppBase *pApp)
    {
        if (!mAppList.empty())
        {
            mAppList.back()->Pause();
        }
        mAppList.push_back(pApp);
        mAppList.back()->Enter();
    }

    void AppManager::PopApp(AppBase* pApp)
    {
        if (!mAppList.empty())
        {
            mAppList.back()->Exit();
            mAppList.pop_back();
        }
        if (!mAppList.empty())
        {
            mAppList.back()->Resume();
        }
    }

    bool AppManager::FrameStarted(const FrameEvent& evt)
    {
        if (!mAppList.empty())
        {
            return mAppList.back()->FrameStarted(evt);
        }
        return true;
    }

    bool AppManager::FrameEnded(const FrameEvent& evt)
    {
        if (!mAppList.empty())
        {
            return mAppList.back()->FrameEnded(evt);
        }
        return true;
    }

    bool AppManager::MouseMoved( const OIS::MouseEvent &arg )
    {
        if (!mAppList.empty())
        {
            return mAppList.back()->MouseMoved(arg);
        }
        return true;
    }

    bool AppManager::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (!mAppList.empty())
        {
            return mAppList.back()->MousePressed(arg, id);
        }
        return true;
    }

    bool AppManager::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (!mAppList.empty())
        {
            return mAppList.back()->MouseReleased(arg, id);
        }
        return true;
    }

    bool AppManager::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_ESCAPE)
        {
            ToolKit::GetSingleton()->SetAppRunning(false);
        }
        if (!mAppList.empty())
        {
            return mAppList.back()->KeyPressed(arg);
        }
        return true;
    }

    bool AppManager::KeyReleased( const OIS::KeyEvent &arg )
    {
        if (!mAppList.empty())
        {
            mAppList.back()->KeyReleased(arg);
        }
        return true;
    }

    AppManager::~AppManager(void)
    {
    }
}


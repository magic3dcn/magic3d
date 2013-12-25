//#include "StdAfx.h"
#include "AppManager.h"
#include "ToolKit.h"
#include "../Application/Homepage.h"
//#include "../Application/PrimitiveDetectionApp.h"

namespace MagicCore
{
    AppManager* AppManager::mpAppMgr = NULL;

    AppManager::AppManager(void) : 
        mpCurrentApp(NULL)
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
        EnterApp(new MagicApp::Homepage, "Homepage");
        //EnterApp(new MagicApp::PrimitiveDetectionApp, "PrimitiveDetectionApp");
    }

    void AppManager::Update(float timeElapsed)
    {
        if (mpCurrentApp != NULL)
        {
            mpCurrentApp->Update(timeElapsed);
        }
    }

    void AppManager::EnterApp(AppBase* pApp, std::string name)
    {
        std::map<std::string, AppBase* >::iterator itr = mAppSet.find(name);
        if (itr == mAppSet.end())
        {
            mAppSet[name] = pApp;
        }
        else
        {
            delete pApp;
        }
        if (mpCurrentApp != NULL)
        {
            mpCurrentApp->Exit();
        }
        mpCurrentApp = mAppSet[name];
        mpCurrentApp->Enter();
    }

    bool AppManager::SwitchCurrentApp(std::string name)
    {
        std::map<std::string, AppBase* >::iterator itr = mAppSet.find(name);
        if (itr != mAppSet.end())
        {
            if (mpCurrentApp != NULL)
            {
                mpCurrentApp->Exit();
            }
            mpCurrentApp = itr->second;
            return mpCurrentApp->Enter();
        }
        else
        {
            return false;
        }
    }

    AppBase* AppManager::GetApp(std::string name)
    {
        std::map<std::string, AppBase* >::iterator itr = mAppSet.find(name);
        if (itr != mAppSet.end())
        {
            return itr->second;
        }
        else
        {
            return NULL;
        }
    }

    bool AppManager::FrameStarted(const FrameEvent& evt)
    {
        if (mpCurrentApp != NULL)
        {
            return mpCurrentApp->FrameStarted(evt);
        }
        else
        {
            return true;
        }
    }

    bool AppManager::FrameEnded(const FrameEvent& evt)
    {
        if (mpCurrentApp != NULL)
        {
            return mpCurrentApp->FrameEnded(evt);
        }
        else
        {
            return true;
        }
    }

    bool AppManager::MouseMoved( const OIS::MouseEvent &arg )
    {
        if (mpCurrentApp != NULL)
        {
            return mpCurrentApp->MouseMoved(arg);
        }
        else
        {
            return true;
        }
    }

    bool AppManager::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mpCurrentApp != NULL)
        {
            return mpCurrentApp->MousePressed(arg, id);
        }
        else
        {
            return true;
        }
    }

    bool AppManager::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mpCurrentApp != NULL)
        {
            return mpCurrentApp->MouseReleased(arg, id);
        }
        else
        {
            return true;
        }
    }

    bool AppManager::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_ESCAPE)
        {
            ToolKit::GetSingleton()->SetAppRunning(false);
            return true;
        }

        if (mpCurrentApp != NULL)
        {
            return mpCurrentApp->KeyPressed(arg);
        }
        else
        {
            return true;
        }
    }

    bool AppManager::KeyReleased( const OIS::KeyEvent &arg )
    {
        if (mpCurrentApp != NULL)
        {
            return mpCurrentApp->KeyReleased(arg);
        }
        else
        {
            return true;
        }
    }

    void AppManager::WindowResized( Ogre::RenderWindow* rw )
    {
        if (mpCurrentApp != NULL)
        {
            mpCurrentApp->WindowResized(rw);
        }
    }

    AppManager::~AppManager(void)
    {
        for (std::map<std::string, AppBase* >::iterator itr = mAppSet.begin(); itr != mAppSet.end(); itr++)
        {
            if (itr->second != NULL)
            {
                delete itr->second;
            }
        }
        mAppSet.clear();
        mpCurrentApp = NULL;
    }
}


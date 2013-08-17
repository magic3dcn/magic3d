#pragma once
#include "AppBase.h"
#include <map>
#include <algorithm>
#include <string>

namespace MagicCore
{
    class AppManager
    {
    private:
        static AppManager* mpAppMgr;
        AppManager(void);
    public:
        static AppManager* GetSingleton(void);

        void Init(void);
        void Update(float timeElapsed);
        void EnterApp(AppBase* pApp, std::string name);
        bool SwitchCurrentApp(std::string name);
        AppBase* GetApp(std::string name);

        bool FrameStarted(const FrameEvent& evt);
        bool FrameEnded(const FrameEvent& evt);
        bool MouseMoved( const OIS::MouseEvent &arg );
        bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        bool KeyPressed( const OIS::KeyEvent &arg );
        bool KeyReleased( const OIS::KeyEvent &arg );

        virtual ~AppManager(void);

    private:
        std::map<std::string, AppBase* > mAppSet;
        AppBase* mpCurrentApp;
    };
}


//////#include "StdAfx.h"
#include "Homepage.h"
#include "MyGUI.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"
#include "PrimitiveDetectionApp.h"
#include "ReliefApp.h"
#include "../Common/AppManager.h"

namespace MagicApp
{
    Homepage::Homepage()
    {
    }

    Homepage::~Homepage()
    {
    }

    bool Homepage::Enter()
    {
        MagicLog << "Enter Homepage" << std::endl;
        mUI.Setup();
        return true;
    }

    bool Homepage::Update(float timeElapsed)
    {
        return true;
    }

    bool Homepage::Exit()
    {
        mUI.Shutdown();
        return true;
    }

    bool Homepage::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_P)
        {
            MagicLog << "EnterPrimitiveDetection" << std::endl;
            MagicCore::AppManager::GetSingleton()->EnterApp(new PrimitiveDetectionApp, "PrimitiveDetectionApp");
        }
        else if (arg.key == OIS::KC_R)
        {
            MagicLog << "EnterReliefApp" << std::endl;
            MagicCore::AppManager::GetSingleton()->EnterApp(new ReliefApp, "ReliefApp");
        }
        return true;
    }
}
#include "StdAfx.h"
#include "Homepage.h"
#include "MyGUI.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"
#include "PrimitiveDetectionApp.h"
#include "AugmentedRealityApp.h"
#include "VideoRecordingApp.h"
#include "../Common/AppManager.h"
#include "FaceBeautificationApp.h"

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
        InfoLog << "Enter Homepage" << std::endl;
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
            InfoLog << "EnterPrimitiveDetection" << std::endl;
            MagicCore::AppManager::GetSingleton()->EnterApp(new PrimitiveDetectionApp, "PrimitiveDetectionApp");
        }
        else if (arg.key == OIS::KC_V)
        {
            InfoLog << "EnterAugmentedRealityApp" << std::endl;
            MagicCore::AppManager::GetSingleton()->EnterApp(new AugmentedRealityApp, "AugmentedRealityApp");
        }
        else if (arg.key == OIS::KC_R)
        {
            InfoLog << "EnterVideoRecordingApp" << std::endl;
            MagicCore::AppManager::GetSingleton()->EnterApp(new VideoRecordingApp, "VideoRecordingApp");
        }
        else if (arg.key == OIS::KC_F)
        {
            InfoLog << "EnterFaceBeautificationApp" << std::endl;
            MagicCore::AppManager::GetSingleton()->EnterApp(new FaceBeautificationApp, "FaceBeautificationApp");
        }

        return true;
    }
}
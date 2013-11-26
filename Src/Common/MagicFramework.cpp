//#include "stdafx.h"
#include "MagicFramework.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "ToolKit.h"
#include "ResourceManager.h"
#include "AppManager.h"
#include "GUISystem.h"
#include "LogSystem.h"

namespace MagicCore
{
    MagicFramework::MagicFramework() :
        mTimeAccumulate(0.f),
        mRenderDeltaTime(0.025f)
    {
    }

    MagicFramework::~MagicFramework()
    {
    }

    void MagicFramework::Init()
    {
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "MagicFramework init" << std::endl;
        RenderSystem::GetSingleton()->Init();
        ResourceManager::Init();
        GUISystem::GetSingleton()->Init(RenderSystem::GetSingleton()->GetRenderWindow(), RenderSystem::GetSingleton()->GetSceneManager(), "MyGUIResource");
        InputSystem::GetSingleton()->Init(RenderSystem::GetSingleton()->GetRenderWindow());
        AppManager::GetSingleton()->Init();
    }

    void MagicFramework::Run()
    {
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "MagicFramework run" << std::endl;
        float timeLastFrame = ToolKit::GetTime();
        while (Running())
        {
            float timeCurrentFrame = ToolKit::GetTime();
            float timeSinceLastFrame = timeCurrentFrame - timeLastFrame;
            timeLastFrame = timeCurrentFrame;
            Update(timeSinceLastFrame);
        }
    }

    void MagicFramework::Update(float timeElapsed)
    {
        //MagicLog(MagicCore::LOGLEVEL_DEBUG) << "MagicFramework update: " << mTimeAccumulate << std::endl;
        InputSystem::GetSingleton()->Update();
        AppManager::GetSingleton()->Update(timeElapsed);
        mTimeAccumulate += timeElapsed;
        if (mTimeAccumulate > mRenderDeltaTime)
        {
            mTimeAccumulate = mTimeAccumulate - mRenderDeltaTime;
            RenderSystem::GetSingleton()->Update();
        }
    }

    bool MagicFramework::Running(void)
    {
        return ToolKit::GetSingleton()->IsAppRunning();
    }
}
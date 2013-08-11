#include "StdAfx.h"
#include "Homepage.h"
#include "MyGUI.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"

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
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/Homepage", "FileSystem", "Homepage");
        MyGUI::LayoutManager::getInstance().loadLayout("HomeLayout.layout");
        return true;
    }

    bool Homepage::Update(float timeElapsed)
    {
        return true;
    }

    bool Homepage::Exit()
    {
        MagicCore::ResourceManager::GetSingleton()->UnloadResource("Homepage");
        return true;
    }

    bool Homepage::Pause()
    {
        return true;
    }

    bool Homepage::Resume()
    {
        return true;
    }
}
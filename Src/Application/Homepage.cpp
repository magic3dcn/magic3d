//////#include "StdAfx.h"
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
}
#include "StdAfx.h"
#include "LogSystem.h"

namespace MagicCore
{
    LogSystem* LogSystem::mpLogSystem = NULL;

    LogSystem::LogSystem(void)
        : mOFStream("Magic3DLog.txt")
    {
    }

    LogSystem* LogSystem::GetSingleton()
    {
        if (mpLogSystem == NULL)
        {
            mpLogSystem = new LogSystem;
        }
        return mpLogSystem;
    }

    LogSystem::~LogSystem(void)
    {
    }

    std::ofstream& LogSystem::GetOFStream()
    {
        return mOFStream;
    }
}


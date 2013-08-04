#pragma once
#include <fstream>
#include <iostream>

#define MagicLog MagicCore::LogSystem::GetSingleton()->GetOFStream()

namespace MagicCore
{
    class LogSystem
    {
    private:
        static LogSystem* mpLogSystem;
        LogSystem(void);
    public:
        static LogSystem* GetSingleton(void);
        ~LogSystem(void);
        std::ofstream& GetOFStream();
    public:
        std::ofstream mOFStream;
    };
}

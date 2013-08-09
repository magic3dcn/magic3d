#pragma once
#include <fstream>
#include <iostream>

namespace MagicCore
{
    #define MagicLog MagicCore::LogSystem::GetSingleton()->GetOFStream()

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

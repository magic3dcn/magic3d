#pragma once
#include <string>

namespace MagicCore
{
    class ResourceManager
    {
    private:
        ResourceManager();
        static ResourceManager* mpResourceMgr;

    public:
        static ResourceManager* GetSingleton();
        void Init(void);
        void LoadResource(std::string path = "", std::string type = "", std::string groupName = "", bool recursive = false);
        void UnloadResource(std::string groupName);
        ~ResourceManager();
    };
}
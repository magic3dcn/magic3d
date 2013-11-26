#pragma once
#include <string>

namespace MagicCore
{
    class ResourceManager
    {
    public:
        ResourceManager();
        static void Init(void);
        static void LoadResource(std::string path = "", std::string type = "", std::string groupName = "", bool recursive = false);
        static void UnloadResource(std::string groupName);
        ~ResourceManager();
    };
}
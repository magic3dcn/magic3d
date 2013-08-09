#include "StdAfx.h"
#include "ResourceManager.h"
#include "OgreResourceGroupManager.h"

namespace MagicCore
{
    ResourceManager* ResourceManager::mpResourceMgr = NULL;

    ResourceManager::ResourceManager()
    {
    }

    ResourceManager* ResourceManager::GetSingleton()
    {
        if (mpResourceMgr == NULL)
        {
            mpResourceMgr = new ResourceManager;
        }
        return mpResourceMgr;
    }

    void ResourceManager::Init()
    {
        LoadResource("../../Media/MyGUIResource", "FileSystem", "MyGUIResource");
    }

    void ResourceManager::LoadResource(std::string path, std::string type, std::string groupName, bool recursive)
    {
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, type, groupName, recursive);
        Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(groupName);
    }

    void ResourceManager::UnloadResource(std::string groupName)
    {
        Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(groupName);
    }
}
#include "StdAfx.h"
#include "GUISystem.h"

namespace MagicCore
{
    GUISystem* GUISystem::mpGUISys = NULL;

    GUISystem::GUISystem() :
        mpGUI(NULL),
        mpPlatform(NULL)
    {

    }

    GUISystem* GUISystem::GetSingleton()
    {
        if (mpGUISys == NULL)
        {
            mpGUISys = new GUISystem;
        }
        return mpGUISys;
    }

    GUISystem::~GUISystem()
    {
    }

    void GUISystem::Init(Ogre::RenderWindow* pWin, Ogre::SceneManager* pMgr, std::string resourceName)
    {
        mpPlatform = new MyGUI::OgrePlatform();
        mpPlatform->initialise(pWin, pMgr, resourceName);
        mpGUI = new MyGUI::Gui();
        mpGUI->initialise();
    }
}
#pragma once
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"
#include "Ogre.h"
#include <string>

namespace MagicCore
{
    class GUISystem
    {
    private:
        GUISystem();
        static GUISystem* mpGUISys;
    public:
        static GUISystem* GetSingleton();
        void Init(Ogre::RenderWindow* pWin, Ogre::SceneManager* pMgr, std::string resourceName);
        ~GUISystem();

    private:
        MyGUI::Gui* mpGUI;
        MyGUI::OgrePlatform* mpPlatform;
    };
}
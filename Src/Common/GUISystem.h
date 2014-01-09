#pragma once
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"
//#include "../Common/MagicOgre.h"
#include <string>

namespace Ogre
{
    class SceneManager;
    class RenderWindow;
}

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
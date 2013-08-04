#pragma once
#include "CEGUI\CEGUI.h"
#include "CEGUI\RendererModules\Ogre\Renderer.h"

namespace MagicCore
{
    class GUISystem
    {
    private:
        GUISystem();
        static GUISystem* mpGUISys;
    public:
        static GUISystem* GetSingleton();
        void Init();
        ~GUISystem();

    private:
        CEGUI::OgreRenderer* mpRenderer;
    };
}
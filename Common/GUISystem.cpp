#include "StdAfx.h"
#include "GUISystem.h"

namespace MagicCore
{
    GUISystem* GUISystem::mpGUISys = NULL;

    GUISystem::GUISystem() :
        mpRenderer(NULL)
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

    void GUISystem::Init()
    {
        mpRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
      //  CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
      //  CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
    }
}
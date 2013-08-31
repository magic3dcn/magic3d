#include "StdAfx.h"
#include "Reconstruction.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"

namespace MagicApp
{
    Reconstruction::Reconstruction()
    {

    }

    Reconstruction::~Reconstruction()
    {

    }

    bool Reconstruction::Enter(void)
    {
        MagicLog << "Enter Reconstruction" << std::endl;
        mUI.Setup();
        SetupScene();
        return true;
    }

    bool Reconstruction::Update(float timeElapsed)
    {
        return true;
    }
    
    bool Reconstruction::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();
        return true;
    }

    bool Reconstruction::MouseMoved( const OIS::MouseEvent &arg )
    {
        mViewTool.MouseMoved(arg);
        return true;
    }

    bool Reconstruction::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        mViewTool.MousePressed(arg);
        return true;
    }

    void Reconstruction::SetupScene(void)
    {
        MagicLog << "Reconstruction::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light*  sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(10, 10, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void Reconstruction::ShutdownScene(void)
    {
        MagicLog << "Reconstruction::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
    }
}
#include "MeshShopApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"

namespace MagicApp
{
    MeshShopApp::MeshShopApp()
    {
    }

    MeshShopApp::~MeshShopApp()
    {
    }

    bool MeshShopApp::Enter(void)
    {
        InfoLog << "Enter MeshShopApp" << std::endl; 
        mUI.Setup();
        SetupScene();

        return true;
    }

    bool MeshShopApp::Update(float timeElapsed)
    {
        return true;
    }

    bool MeshShopApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();
        return true;
    }

    bool MeshShopApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        return true;
    }

    bool MeshShopApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool MeshShopApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        return true;
    }

    void MeshShopApp::SetupScene(void)
    {
        InfoLog << "MeshShopApp::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light* sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(10, 10, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void MeshShopApp::ShutdownScene(void)
    {
        InfoLog << "MeshShopApp::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        //MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("RenderOBJ");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetToInitialState();
    }
}
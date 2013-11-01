#include "ReliefApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"

namespace MagicApp
{
    ReliefApp::ReliefApp ()
    {
    }

    ReliefApp::~ReliefApp ()
    {
    }

    bool ReliefApp::Enter(void)
    {
        MagicLog << "Enter ReliefApp" << std::endl;
        mUI.Setup();
        SetupScene();
        return true;
    }

    bool ReliefApp::Update(float timeElapsed)
    {
        return true;
    }

    bool ReliefApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();
        return true;
    }

    bool ReliefApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        mViewTool.MouseMoved(arg);
        return true;
    }

    bool ReliefApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        mViewTool.MousePressed(arg);
        return true;
    }

    bool ReliefApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    void ReliefApp::SetupScene(void)
    {
        MagicLog << "ReliefApp::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light*  sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(10, 10, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void ReliefApp::ShutdownScene(void)
    {
        MagicLog << "ReliefApp::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetToInitialState();
    }
}
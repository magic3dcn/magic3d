#include "PrimitiveDetectionApp.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"
#include "../Common/RenderSystem.h"

namespace MagicApp
{
    PrimitiveDetectionApp::PrimitiveDetectionApp() : 
        mpMesh(NULL)
    {
    }

    PrimitiveDetectionApp::~PrimitiveDetectionApp()
    {
        if (mpMesh != NULL)
        {
            delete mpMesh;
            mpMesh = NULL;
        }
    }

    bool PrimitiveDetectionApp::Enter(void)
    {
        MagicLog << "Enter PrimitiveDetectionApp" << std::endl;
        mUI.Setup();
        SetupScene();
        return true;
    }

    bool PrimitiveDetectionApp::Update(float timeElapsed)
    {
        return true;
    }

    bool PrimitiveDetectionApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();
        return true;
    }

    bool PrimitiveDetectionApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        mViewTool.MouseMoved(arg);
        return true;
    }

    bool PrimitiveDetectionApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        mViewTool.MousePressed(arg);
        return true;
    }

    void PrimitiveDetectionApp::SetupScene(void)
    {
        MagicLog << "PrimitiveDetectionApp::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light*  sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(10, 10, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }
     
    void PrimitiveDetectionApp::ShutdownScene(void)
    {
        MagicLog << "PrimitiveDetectionApp::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
    }

    void PrimitiveDetectionApp::SetMesh3D(MagicDGP::Mesh3D* pMesh)
    {
        if (mpMesh != NULL)
        {
            delete mpMesh;
        }
        mpMesh = pMesh;
    }
}
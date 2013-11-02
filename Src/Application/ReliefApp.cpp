#include "ReliefApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../DGP/Relief.h"

namespace MagicApp
{
    ReliefApp::ReliefApp() : 
        mpPointSet(NULL),
        mpMesh(NULL)
    {
    }

    ReliefApp::~ReliefApp()
    {
        if (mpPointSet != NULL)
        {
            delete mpPointSet;
            mpPointSet = NULL;
        }
        if (mpMesh != NULL)
        {
            delete mpMesh;
            mpMesh = NULL;
        }
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
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("RenderOBJ");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetToInitialState();
    }

    bool ReliefApp::ImportPointSet()
    {
        std::string fileName;
        char filterName[] = "OBJ Files(*.obj)\0*.obj\0";
        if (MagicCore::ToolKit::GetSingleton()->FileOpenDlg(fileName, filterName))
        {
            MagicDGP::Point3DSet* pPointSet = MagicDGP::Parser::ParsePointSet(fileName);
            if (pPointSet != NULL)
            {
                pPointSet->UnifyPosition(2.0);
                if (mpPointSet != NULL)
                {
                    delete mpPointSet;
                }
                mpPointSet = pPointSet;
                MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderOBJ", "SimplePoint", mpPointSet);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    void ReliefApp::GenerateRelief()
    {
        MagicDGP::ReliefGeneration reliefGen(512, 512, -1, 1, -1, 1);
        MagicDGP::Mesh3D* pMesh = reliefGen.GenerationFromPointClout(mpPointSet);
        if (pMesh != NULL)
        {
            if (mpMesh != NULL)
            {
                delete mpMesh;
            }
            mpMesh = pMesh;
            MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderOBJ", "MyCookTorrance", mpMesh);
        }
    }
}
#include "PrimitiveDetectionApp.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"
#include "../Common/RenderSystem.h"
#include "../DGP/PrimitiveDetection.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"

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

    bool PrimitiveDetectionApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool PrimitiveDetectionApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_R && mpMesh !=NULL)
        {
            MagicLog << "Reverse Mesh Normal" << std::endl;
            mpMesh->ReverseNormal();
            MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("TestMesh3D", "MyCookTorrance", mpMesh);
        }
        if (arg.key == OIS::KC_V && mpMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_POINTS);
        }
        if (arg.key == OIS::KC_E && mpMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_WIREFRAME);
        }
        if (arg.key == OIS::KC_F && mpMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_SOLID);
        }

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
        //Ogre::Light*  sb = pSceneMgr->createLight("SimpleLightBack");
        //sb->setPosition(10, 10, -20);
        //sb->setDiffuseColour(0.8, 0.8, 0.8);
        //sb->setSpecularColour(0.5, 0.5, 0.5);

    }
     
    void PrimitiveDetectionApp::ShutdownScene(void)
    {
        MagicLog << "PrimitiveDetectionApp::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        if (mpMesh != NULL)
        {
            delete mpMesh;
            mpMesh = NULL;
        }
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("Mesh3D");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetOrientation();
        //pSceneMgr->destroyLight("SimpleLightBack");
    }

    void PrimitiveDetectionApp::SetMesh3D(MagicDGP::Mesh3D* pMesh)
    {
        if (mpMesh != NULL)
        {
            delete mpMesh;
        }
        mpMesh = pMesh;
    }

    bool PrimitiveDetectionApp::ImportMesh3D()
    {
        std::string fileName;
        if (MagicCore::ToolKit::GetSingleton()->FileOpenDlg(fileName))
        {
            MagicDGP::Mesh3D* pMesh = MagicDGP::Parser::ParseMesh3D(fileName);
            if (pMesh != NULL)
            {
                pMesh->UpdateNormal();
                pMesh->UnifyPosition(2.0);
                if (mpMesh != NULL)
                {
                    delete mpMesh;
                }
                mpMesh = pMesh;
                MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
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

    void PrimitiveDetectionApp::RansacPrimitiveDetection()
    {
        std::vector<int> res;
        //MagicDGP::PrimitiveDetection::Primitive2DDetectionFromRansac(mpMesh, res);
        MagicDGP::PrimitiveDetection::Primitive2DDetectionTest(mpMesh, res);
        int vertNum = mpMesh->GetVertexNumber();
        for (int i = 0; i < vertNum; i++)
        {
            float cv = res.at(i) * 0.2f;
            MagicDGP::Vector3 color = MagicCore::ToolKit::GetSingleton()->ColorCoding(cv);
            mpMesh->GetVertex(i)->SetColor(color);
        }
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
    }
}
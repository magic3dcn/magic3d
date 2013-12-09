#include "MeshShopApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../DGP/Consolidation.h"

namespace MagicApp
{
    MeshShopApp::MeshShopApp() :
        mpMesh(NULL)
    {
    }

    MeshShopApp::~MeshShopApp()
    {
        if (mpMesh != NULL)
        {
            delete mpMesh;
            mpMesh = NULL;
        }
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
        mViewTool.MouseMoved(arg);
        return true;
    }

    bool MeshShopApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        mViewTool.MousePressed(arg);
        return true;
    }

    bool MeshShopApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_V && mpMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_POINTS);
        }
        else if (arg.key == OIS::KC_E && mpMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_WIREFRAME);
        }
        else if (arg.key == OIS::KC_F && mpMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_SOLID);
        }

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
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("RenderMesh");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetToInitialState();
    }

    bool MeshShopApp::OpenMesh(int& vertNum)
    {
        std::string fileName;
        char filterName[] = "OBJ Files(*.obj)\0*.obj\0STL Files(*.stl)\0*.stl\0OFF Files(*.off)\0*.off\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            MagicDGP::Mesh3D* pMesh = MagicDGP::Parser::ParseMesh3D(fileName);
            if (pMesh != NULL)
            {
                vertNum = pMesh->GetVertexNumber();
                pMesh->UnifyPosition(2.0);
                pMesh->UpdateNormal();
                if (mpMesh != NULL)
                {
                    delete mpMesh;
                }
                mpMesh = pMesh;
                MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderMesh", "MyCookTorrance", mpMesh);
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

    void MeshShopApp::SaveMesh()
    {
        if (mpMesh != NULL)
        {
            std::string fileName;
            char filterName[] = "Support format(*.obj, *.stl, *.off)\0*.*\0";
            if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
            {
                MagicDGP::Parser::ExportMesh3D(fileName, mpMesh);
            }
        }
    }

    void MeshShopApp::SmoothMesh()
    {
        //MagicDGP::Consolidation::MeanCurvatureFlowFairing(mpMesh);
        MagicDGP::Consolidation::SimpleMeshSmooth(mpMesh);
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderMesh", "MyCookTorrance", mpMesh);
    }

    void MeshShopApp::SimplifyMesh()
    {

    }

    void MeshShopApp::RemoveOutlier()
    {
        MagicDGP::Mesh3D* pNewMesh = MagicDGP::Consolidation::RemoveSmallMeshPatch(mpMesh, 0.1);
        if (pNewMesh != NULL)
        {
            delete mpMesh;
            mpMesh = pNewMesh;
            MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderMesh", "MyCookTorrance", mpMesh);
        }
        else
        {
            DebugLog << "MeshShopApp::RemoveOutlier:: failed" << std::endl;
        }
    }

    void MeshShopApp::AddNoise()
    {
        int vertNum = mpMesh->GetVertexNumber();
        int maxNum = 100;
        MagicDGP::Real epsilon = 0.001;
        for (int vid = 0; vid < vertNum; vid++)
        {
            int randNum = rand();
            randNum = randNum % (2 * maxNum) - maxNum;
            MagicDGP::Real scale = MagicDGP::Real(randNum) / maxNum * epsilon;
            MagicDGP::Vector3 newPos = mpMesh->GetVertex(vid)->GetPosition() + mpMesh->GetVertex(vid)->GetNormal() * scale;
            mpMesh->GetVertex(vid)->SetPosition(newPos);
        }
        mpMesh->UpdateNormal();
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderMesh", "MyCookTorrance", mpMesh);
    }

    void MeshShopApp::RectangleSelect()
    {

    }

    void MeshShopApp::CycleSelect()
    {

    }

    void MeshShopApp::IntelligentSelect()
    {

    }

    void MeshShopApp::DeformMesh()
    {

    }

    void MeshShopApp::SetupFromMeshInput(MagicDGP::Mesh3D* pMesh)
    {
        if (mpMesh != NULL)
        {
            delete mpMesh;
        }
        mpMesh = pMesh;
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderMesh", "MyCookTorrance", mpMesh);
        mUI.SetupFromMeshInput(mpMesh->GetVertexNumber());
    }
}
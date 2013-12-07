#include "PointShopApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../DGP/Consolidation.h"
#include "../DGP/Filter.h"
#include "../DGP/MeshReconstruction.h"
#include "../Common/AppManager.h"
#include "../Application/MeshShopApp.h"

namespace MagicApp
{
    PointShopApp::PointShopApp() :
        mpPointSet(NULL)
    {
    }

    PointShopApp::~PointShopApp()
    {
        if (mpPointSet != NULL)
        {
            delete mpPointSet;
            mpPointSet = NULL;
        }
    }

    bool PointShopApp::Enter(void)
    {
        InfoLog << "Enter PointShopApp" << std::endl; 
        mUI.Setup();
        SetupScene();

        return true;
    }

    bool PointShopApp::Update(float timeElapsed)
    {
        return true;
    }

    bool PointShopApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();
        return true;
    }

    bool PointShopApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        mViewTool.MouseMoved(arg);
        return true;
    }

    bool PointShopApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        mViewTool.MousePressed(arg);
        return true;
    }

    bool PointShopApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        return true;
    }

    bool PointShopApp::OpenPointSet(bool& hasNormal, int& pointNum)
    {
        std::string fileName;
        char filterName[] = "OBJ Files(*.obj)\0*.obj\0STL Files(*.stl)\0*.stl\0OFF Files(*.off)\0*.off\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            MagicDGP::Point3DSet* pPointSet = MagicDGP::Parser::ParsePointSet(fileName);
            if (pPointSet != NULL)
            {
                hasNormal = pPointSet->HasNormal();
                pointNum = pPointSet->GetPointNumber();
                pPointSet->UnifyPosition(2.0);
                if (mpPointSet != NULL)
                {
                    delete mpPointSet;
                }
                mpPointSet = pPointSet;
                UpdatePointSetRendering();
                
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

    void PointShopApp::SavePointSet()
    {
        if (mpPointSet != NULL)
        {
            std::string fileName;
            char filterName[] = "Support format(*.obj, *.off, *.ply)\0*.*\0";
            if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
            {
                MagicDGP::Parser::ExportPointSet(fileName, mpPointSet);
            }
        }
    }

    void PointShopApp::CalPointSetNormal()
    {
        if (mpPointSet->HasNormal() == true)
        {
            MagicDGP::Consolidation::RedressPointSetNormal(mpPointSet);
        }
        else
        {
            MagicDGP::Consolidation::CalPointSetNormal(mpPointSet);
        }
        UpdatePointSetRendering();
    }

    void PointShopApp::FlipPointSetNormal()
    {
        int pointNum = mpPointSet->GetPointNumber();
        for (int pid = 0; pid < pointNum; pid++)
        {
            MagicDGP::Vector3 nor = mpPointSet->GetPoint(pid)->GetNormal();
            nor *= -1;
            mpPointSet->GetPoint(pid)->SetNormal(nor);
        }
        UpdatePointSetRendering();
    }

    void PointShopApp::SmoothPointSet()
    {

    }

    bool PointShopApp::SamplePointSet(int sampleNum)
    {
        MagicDGP::Point3DSet* pSamplePointSet = MagicDGP::Filter::PointSetSampling(mpPointSet, sampleNum);
        if (pSamplePointSet != NULL)
        {
            //transfer property
            pSamplePointSet->SetHasNormal(mpPointSet->HasNormal());
            //
            delete mpPointSet;
            mpPointSet = pSamplePointSet;
            UpdatePointSetRendering();
            return true;
        }
        else
        {
            return false;
        }
    }

    void PointShopApp::RemoveOutlier()
    {
        if (mpPointSet != NULL)
        {
            MagicDGP::Point3DSet* pNewPS = MagicDGP::Filter::RemovePointSetOutlier(mpPointSet, 0.02);
            if (pNewPS != NULL)
            {
                //transfer property
                pNewPS->SetHasNormal(mpPointSet->HasNormal());
                //
                delete mpPointSet;
                mpPointSet = pNewPS;
                UpdatePointSetRendering();
            }
        }
    }

    void PointShopApp::Reconstruction()
    {
        mpPointSet->CalculateBBox();
        mpPointSet->CalculateDensity();
        MagicDGP::Mesh3D* pNewMesh = MagicDGP::MeshReconstruction::ScreenPoissonReconstruction(mpPointSet);
        if (pNewMesh != NULL)
        {
            //pNewMesh->UnifyPosition(2);
            //pNewMesh->UpdateNormal();
            MagicCore::AppManager::GetSingleton()->EnterApp(new MeshShopApp, "MeshShopApp");
            MeshShopApp* pMSApp = dynamic_cast<MeshShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MeshShopApp"));
            if (pMSApp != NULL)
            {
                pMSApp->SetupFromMeshInput(pNewMesh);
            }
            else
            {
                WarnLog << "Get MeshShopApp Failed" << std::endl;
            }
        }
    }

    void PointShopApp::AddNoise()
    {

    }

    void PointShopApp::RectangleSelect()
    {

    }

    void PointShopApp::CycleSelect()
    {

    }

    void PointShopApp::IntelligentSelect()
    {

    }

    void PointShopApp::DeformPointSet()
    {

    }

    void PointShopApp::SetupScene(void)
    {
        InfoLog << "PointShopApp::SetupScene" << std::endl;
        //MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getViewport(0)->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light* sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(0, 0, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void PointShopApp::ShutdownScene(void)
    {
        InfoLog << "PointShopApp::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("RenderPointSet");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetToInitialState();
    }

    void PointShopApp::UpdatePointSetRendering()
    {
        if (mpPointSet->HasNormal())
        {
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderPointSet", "MyCookTorrancePoint", mpPointSet);
        }
        else
        {
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderPointSet", "SimplePoint", mpPointSet);
        }
    }

    void PointShopApp::SetupFromPointsetInput(MagicDGP::Point3DSet* pPS)
    {
        if (mpPointSet != NULL)
        {
            delete mpPointSet;
        }
        mpPointSet = pPS;
        mpPointSet->UnifyPosition(2);
        UpdatePointSetRendering();
        mUI.SetupFromPointsetInput(mpPointSet->HasNormal(), mpPointSet->GetPointNumber());
    }
}
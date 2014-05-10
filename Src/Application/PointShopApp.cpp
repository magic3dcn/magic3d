#include "stdafx.h"
#include "PointShopApp.h"
#include "../Tool/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../DGP/Consolidation.h"
#include "../DGP/Sampling.h"
#include "../DGP/MeshReconstruction.h"
#include "../Common/AppManager.h"
#include "../Application/MeshShopApp.h"
//#include "../Common/MagicOgre.h"

namespace MagicApp
{
    PointShopApp::PointShopApp() :
        mpPointSet(NULL),
        mMouseMode(MM_View),
        mPickIgnoreBack(true)
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
        ModelViewer();

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
        if (mMouseMode == MM_View)
        {
            MagicDGP::ViewTool::MouseMode mm;
            if (arg.state.buttonDown(OIS::MB_Left))
            {
                mm = MagicDGP::ViewTool::MM_Left_Down;
            }
            else if (arg.state.buttonDown(OIS::MB_Middle))
            {
                mm = MagicDGP::ViewTool::MM_Middle_Down;
            }
            else if (arg.state.buttonDown(OIS::MB_Right))
            {
                mm = MagicDGP::ViewTool::MM_Right_Down;
            }
            else
            {
                mm = MagicDGP::ViewTool::MM_None;
            }
            mViewTool.MouseMoved(arg.state.X.abs, arg.state.Y.abs, mm);
        }
        else if ((mMouseMode == MM_Pick_Rectangle || mMouseMode == MM_Pick_Cycle) && arg.state.buttonDown(OIS::MB_Left))
        {
            mPickTool.MouseMoved(arg.state.X.abs, arg.state.Y.abs);
        }
        
        return true;
    }

    bool PointShopApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_View)
        {
            mViewTool.MousePressed(arg.state.X.abs, arg.state.Y.abs);
        }
        else if (mMouseMode == MM_Pick_Rectangle)
        {
            mPickTool.SetPickParameter(MagicDGP::PM_Rectangle, mPickIgnoreBack, NULL, NULL, mpPointSet);
            mPickTool.MousePressed(arg.state.X.abs, arg.state.Y.abs);
        }
        else if (mMouseMode == MM_Pick_Cycle)
        {
            mPickTool.SetPickParameter(MagicDGP::PM_Cycle, mPickIgnoreBack, NULL, NULL, mpPointSet);
            mPickTool.MousePressed(arg.state.X.abs, arg.state.Y.abs);
        }

        return true;
    }

    bool PointShopApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_Pick_Rectangle || mMouseMode == MM_Pick_Cycle)
        {
            mPickTool.MouseReleased(arg.state.X.abs, arg.state.Y.abs);
            std::vector<int> pickIndex;
            mPickTool.GetPickPointsetIndex(pickIndex);
            MagicMath::Vector3 pickColor(1 - mDefaultColor[0], 1 - mDefaultColor[1], 1 - mDefaultColor[2]);
            for (std::vector<int>::iterator piIter = pickIndex.begin(); piIter != pickIndex.end(); ++piIter)
            {
                mPickIndexSet.insert(*piIter);
                mpPointSet->GetPoint(*piIter)->SetColor(pickColor);
            }
            UpdatePointSetRendering();
        }

        return true;
    }

    bool PointShopApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_BACK || arg.key == OIS::KC_DELETE)
        {
            DeleteSelcetPoints();
        }
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
                if (pPointSet->GetPointNumber() > 0)
                {
                    mDefaultColor = pPointSet->GetPoint(0)->GetColor();
                }
                hasNormal = pPointSet->HasNormal();
                pointNum = pPointSet->GetPointNumber();
                pPointSet->UnifyPosition(2.0);
                if (mpPointSet != NULL)
                {
                    delete mpPointSet;
                }
                mpPointSet = pPointSet;
                UpdatePointSetRendering();
                ClearSceneData();
                ModelViewer();

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
            MagicMath::Vector3 nor = mpPointSet->GetPoint(pid)->GetNormal();
            nor *= -1;
            mpPointSet->GetPoint(pid)->SetNormal(nor);
        }
        UpdatePointSetRendering();
    }

    void PointShopApp::SmoothPointSet()
    {
        if (mRiemannianGraph.size() > 0)
        {
            MagicDGP::Consolidation::SimplePointsetSmooth(mpPointSet, mRiemannianGraph, false);
        }
        else
        {
            MagicDGP::Consolidation::SimplePointsetSmooth(mpPointSet, mRiemannianGraph, true);
        }
        
        UpdatePointSetRendering();
    }

    bool PointShopApp::SamplePointSet(int sampleNum)
    {
        MagicDGP::Point3DSet* pSamplePointSet = MagicDGP::Sampling::PointSetUniformSampling(mpPointSet, sampleNum);
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
            MagicDGP::Point3DSet* pNewPS = MagicDGP::Consolidation::RemovePointSetOutlier(mpPointSet, 0.02);
            if (pNewPS != NULL)
            {
                //transfer property
                pNewPS->UnifyPosition(2);
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
        MagicDGP::LightMesh3D* pNewMesh = MagicDGP::MeshReconstruction::ScreenPoissonReconstruction(mpPointSet);
        if (pNewMesh != NULL)
        {
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
        int pointNum = mpPointSet->GetPointNumber();
        int maxNum = 100;
        double epsilon = 0.001;
        for (int pid = 0; pid < pointNum; pid++)
        {
            int randNum = rand();
            randNum = randNum % (2 * maxNum) - maxNum;
            double scale = double(randNum) / maxNum * epsilon;
            MagicMath::Vector3 newPos = mpPointSet->GetPoint(pid)->GetPosition() + mpPointSet->GetPoint(pid)->GetNormal() * scale;
            mpPointSet->GetPoint(pid)->SetPosition(newPos);
        }
        UpdatePointSetRendering();
    }

    void PointShopApp::RectangleSelect()
    {
        mMouseMode = MM_Pick_Rectangle;
    }

    void PointShopApp::CycleSelect()
    {
        mMouseMode = MM_Pick_Cycle;
    }

    void PointShopApp::ClearSelect()
    {
        for (std::set<int>::iterator pickItr = mPickIndexSet.begin(); pickItr != mPickIndexSet.end(); pickItr++)
        {
            mpPointSet->GetPoint(*pickItr)->SetColor(mDefaultColor);
        }
        mPickIndexSet.clear();
        UpdatePointSetRendering();
    }

    void PointShopApp::ModelViewer()
    {
        mMouseMode = MM_View;
    }

    void PointShopApp::SetupScene(void)
    {
        InfoLog << "PointShopApp::SetupScene" << std::endl;
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
        if (MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasSceneNode("ModelNode"))
        {
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->resetToInitialState();
        } 
        ClearSceneData();
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

    void PointShopApp::DeleteSelcetPoints()
    {
        if (mpPointSet != NULL && mPickIndexSet.size() > 0)
        {
            for (std::set<int>::iterator pickItr = mPickIndexSet.begin(); pickItr != mPickIndexSet.end(); ++pickItr)
            {
                mpPointSet->GetPoint(*pickItr)->SetValid(false);
            }
            //Update to new point set
            MagicDGP::Point3DSet* pNewPointSet = new MagicDGP::Point3DSet;
            int pointNum = mpPointSet->GetPointNumber();
            for (int pid = 0; pid < pointNum; pid++)
            {
                MagicDGP::Point3D* pPoint = mpPointSet->GetPoint(pid);
                if (pPoint->IsValid() == true)
                {
                    MagicDGP::Point3D* pNewPoint = new MagicDGP::Point3D(pPoint->GetPosition(), pPoint->GetNormal());
                    pNewPointSet->InsertPoint(pNewPoint);
                }
            }
            pNewPointSet->UnifyPosition(2);
            pNewPointSet->SetHasNormal(mpPointSet->HasNormal());
            delete mpPointSet;
            mpPointSet = pNewPointSet;
            ClearSceneData();
            UpdatePointSetRendering();
        }
    }

    void PointShopApp::SetPickIgnoreBack(bool ignore)
    {
        mPickIgnoreBack = ignore;
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
        ClearSceneData();
        mUI.SetupFromPointsetInput(mpPointSet->HasNormal(), mpPointSet->GetPointNumber());
    }

    void PointShopApp::ClearSceneData()
    {
        mPickIndexSet.clear();
        mRiemannianGraph.clear();
    }
}
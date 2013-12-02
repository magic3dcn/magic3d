////#include "StdAfx.h"
#include "PointSetViewer.h"
#include "MyGUI.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../Common/RenderSystem.h"
#include "../DGP/Sampling.h"
#include "../DGP/MeshReconstruction.h"
#include "../DGP/Filter.h"
#include "flann/flann.h"

namespace MagicApp
{
    PointSetViewer::PointSetViewer() :
        mpPointSet(NULL),
        mpMesh(NULL)
    {
    }

    PointSetViewer::~PointSetViewer()
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

    bool PointSetViewer::Enter()
    {
        InfoLog << "Enter PointSetViewer" << std::endl;
        mUI.Setup();
        SetupScene();
        return true;
    }

    bool PointSetViewer::Update(float timeElapsed)
    {
        return true;
    }

    bool PointSetViewer::Exit()
    {
        ShutdownScene();
        mUI.Shutdown();
        return true;
    }

    void PointSetViewer::SetupScene()
    {
        InfoLog << "PointSetViewer::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light*  sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(10, 10, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void PointSetViewer::ShutdownScene()
    {
        InfoLog << "PointSetViewer::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("RenderOBJ");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetToInitialState();
    }

    bool PointSetViewer::MouseMoved( const OIS::MouseEvent &arg )
    {
        mViewTool.MouseMoved(arg);
        return true;
    }

    bool PointSetViewer::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        mViewTool.MousePressed(arg);
        return true;
    }

    bool PointSetViewer::KeyPressed( const OIS::KeyEvent &arg )
    {
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
        if (arg.key == OIS::KC_S && mpPointSet !=NULL)
        {
            SamplePointSet();
        }
        return true;
    }

    bool PointSetViewer::ImportPointSet()
    {
        std::string fileName;
        char filterName[] = "OBJ Files(*.obj)\0*.obj\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
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
                //MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderOBJ", "SimplePoint", mpPointSet);
                MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderOBJ", "MyCookTorrancePoint", mpPointSet);
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

    void PointSetViewer::ExportPointSet()
    {
        if (mpPointSet != NULL)
        {
            std::string fileName;
            char filterName[] = "OBJ Files(*.obj)\0*.obj\0";
            if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
            {
                MagicDGP::Parser::ExportPointSet(fileName, mpPointSet);
            }
        }
    }

    bool PointSetViewer::ImportMesh3D()
    {
        std::string fileName;
        char filterName[] = "OBJ Files(*.obj)\0*.obj\0STL Files(*.stl)\0*.stl\0OFF Files(*.off)\0*.off\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            MagicDGP::Mesh3D* pMesh = MagicDGP::Parser::ParseMesh3D(fileName);
            if (pMesh != NULL)
            {
                pMesh->UnifyPosition(2.0);
                pMesh->UpdateNormal();
                if (mpMesh != NULL)
                {
                    delete mpMesh;
                }
                mpMesh = pMesh;
                MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderOBJ", "MyCookTorrance", mpMesh);
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

    void PointSetViewer::ExportMesh3D()
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

    bool PointSetViewer::ReconstructPointSet()
    {
        mpPointSet->CalculateBBox();
        mpPointSet->CalculateDensity();
        MagicDGP::Mesh3D* pNewMesh = MagicDGP::MeshReconstruction::ScreenPoissonReconstruction(mpPointSet);
        if (pNewMesh != NULL)
        {
            if (mpMesh != NULL)
            {
                delete mpMesh;
            }
            mpMesh = pNewMesh;
            MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderOBJ", "MyCookTorrance", mpMesh);
            return true;
        }
        else
        {
            return false;
        }
    }

    void PointSetViewer::FilterPointSetOutliers()
    {
        if (mpPointSet != NULL)
        {
            MagicDGP::Point3DSet* pNewPS = MagicDGP::Filter::RemovePointSetOutlier(mpPointSet, 0.02);
            if (pNewPS != NULL)
            {
                delete mpPointSet;
                mpPointSet = pNewPS;
                MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderOBJ", "SimplePoint", mpPointSet);
            }
        }
        
    }

    void PointSetViewer::SamplePointSet()
    {
        if (mpPointSet != NULL)
        {
            MagicDGP::Point3DSet* pNewPS = MagicDGP::Filter::PointSetSampling(mpPointSet, 10000);
            if (pNewPS != NULL)
            {
                delete mpPointSet;
                mpPointSet = pNewPS;
                MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderOBJ", "SimplePoint", mpPointSet);
            }
        }
    }

    bool PointSetViewer::FilterMesh3D()
    {
        MagicDGP::Filter::SimpleMeshSmooth(mpMesh);
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderOBJ", "MyCookTorrance", mpMesh);
        return true;
    }
}
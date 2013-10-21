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
        MagicLog << "Enter PointSetViewer" << std::endl;
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
        MagicLog << "PointSetViewer::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light*  sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(10, 10, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void PointSetViewer::ShutdownScene()
    {
        MagicLog << "PointSetViewer::ShutdownScene" << std::endl;
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

    bool PointSetViewer::ImportPointSet()
    {
        std::string fileName;
        if (MagicCore::ToolKit::GetSingleton()->FileOpenDlg(fileName))
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

    void PointSetViewer::ExportPointSet()
    {
        if (mpPointSet != NULL)
        {
            std::string fileName;
            if (MagicCore::ToolKit::GetSingleton()->FileSaveDlg(fileName))
            {
                MagicDGP::Parser::ExportPointSet(fileName, mpPointSet);
            }
        }
    }

    bool PointSetViewer::ImportMesh3D()
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
            if (MagicCore::ToolKit::GetSingleton()->FileSaveDlg(fileName))
            {
                MagicDGP::Parser::ExportMesh3D(fileName, mpMesh);
            }
        }
    }

    bool PointSetViewer::FilterPointSet()
    {
        int pointNum = mpPointSet->GetPointNumber();
        /*int sampleNum = pointNum;
        if (pointNum > 200000)
        {
            sampleNum = pointNum / 20;
        }
        else if (pointNum > 10000)
        {
            sampleNum = 10000;
        }*/
        int sampleNum = pointNum / 2;
        mpPointSet->CalculateBBox();
        mpPointSet->CalculateDensity();
        MagicDGP::Point3DSet* pNewPointSet = MagicDGP::Sampling::WLOPSampling(mpPointSet, sampleNum);
        if (pNewPointSet != NULL)
        {
            delete mpPointSet;
            mpPointSet = pNewPointSet;
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderOBJ", "SimplePoint", mpPointSet);
            return true;
        }
        else
        {
            return false;
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

    bool PointSetViewer::FilterMesh3D()
    {
        MagicDGP::Mesh3D* pNewMesh = MagicDGP::Filter::RemoveSmallMeshPatch(mpMesh);
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
}
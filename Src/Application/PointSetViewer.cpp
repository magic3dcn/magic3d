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

        return true;
    }

    bool PointSetViewer::ImportPointSet()
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
                mPSDensityMap.clear();
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
            if (MagicCore::ToolKit::GetSingleton()->FileSaveDlg(fileName, filterName))
            {
                MagicDGP::Parser::ExportPointSet(fileName, mpPointSet);
            }
        }
    }

    bool PointSetViewer::ImportMesh3D()
    {
        std::string fileName;
        char filterName[] = "OBJ Files(*.obj)\0*.obj\0";
        if (MagicCore::ToolKit::GetSingleton()->FileOpenDlg(fileName, filterName))
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
            char filterName[] = "OBJ Files(*.obj)\0*.obj\0";
            if (MagicCore::ToolKit::GetSingleton()->FileSaveDlg(fileName, filterName))
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

    void PointSetViewer::FilterPointSet()
    {
        /*int pcNum = mpPointSet->GetPointNumber();
        mpPointSet->CalculateBBox();
        mpPointSet->CalculateDensity();
        MagicDGP::Point3DSet* pNewPC = MagicDGP::Sampling::WLOPSampling(mpPointSet, pcNum / 2);
        if (pNewPC != NULL)
        {
            delete mpPointSet;
            mpPointSet = pNewPC;
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderOBJ", "SimplePoint", mpPointSet);
        }*/
        //Remove outliers

    }

    void PointSetViewer::FilterPSOutliers(float proportion)
    {
        if (mPSDensityMap.size() == 0)
        {
            int dim = 3;
            int pointNum = mpPointSet->GetPointNumber();
            int refNum = pointNum;
            float* dataSet = new float[refNum * dim];
            int searchNum = pointNum;
            float* searchSet = new float[searchNum * dim];
            for (int i = 0; i < pointNum; i++)
            {
                MagicDGP::Vector3 pos = mpPointSet->GetPoint(i)->GetPosition();
                MagicDGP::Vector3 nor = mpPointSet->GetPoint(i)->GetNormal();
                dataSet[dim * i + 0] = pos[0];
                dataSet[dim * i + 1] = pos[1];
                dataSet[dim * i + 2] = pos[2];
                searchSet[dim * i + 0] = pos[0];
                searchSet[dim * i + 1] = pos[1];
                searchSet[dim * i + 2] = pos[2];
            }
            int nn = 9;
            int* pIndex = new int[searchNum * nn];
            float* pDist = new float[searchNum * nn];
            FLANNParameters searchPara;
            searchPara = DEFAULT_FLANN_PARAMETERS;
            searchPara.algorithm = FLANN_INDEX_KDTREE;
            searchPara.trees = 8;
            searchPara.log_level = FLANN_LOG_INFO;
            searchPara.checks = 64;
            float speedup;
            flann_index_t indexId = flann_build_index(dataSet, refNum, dim, &speedup, &searchPara);
            flann_find_nearest_neighbors_index(indexId, searchSet, searchNum, pIndex, pDist, nn, &searchPara);
            flann_free_index(indexId, &searchPara);
            delete []dataSet;
            delete []searchSet;
            for (int i = 0; i < pointNum; i++)
            {
                MagicDGP::Vector3 pos = mpPointSet->GetPoint(i)->GetPosition();
                MagicDGP::Vector3 nor = mpPointSet->GetPoint(i)->GetNormal();
                float density = 0;
                int baseIndex = nn * i;
                for (int j = 0; j < nn; j++)
                {
                    MagicDGP::Vector3 posNeigh = mpPointSet->GetPoint(pIndex[baseIndex + j])->GetPosition();
                    MagicDGP::Vector3 deltaPos = posNeigh - pos + nor * 1000 * ( (posNeigh - pos) * nor );
                    density += deltaPos.Length();
                }
                mPSDensityMap[density] = i;
            }
            if (pIndex != NULL)
            {
                delete []pIndex;
                pIndex = NULL;
            }
            if (pDist != NULL)
            {
                delete []pDist;
                pDist = NULL;
            }
        }
        int pointNum = mpPointSet->GetPointNumber();
        int invalidNum = pointNum * 0.01;
        int invalidIndex = 0;
        for (std::map<float, int>::reverse_iterator itr = mPSDensityMap.rbegin(); itr != mPSDensityMap.rend(); ++itr)
        {
            if (invalidIndex == invalidNum)
            {
                break;
            }
            mpPointSet->GetPoint(itr->second)->SetValid(false);
            invalidIndex++;
        }
        mPSDensityMap.clear();
        MagicDGP::Point3DSet* pNewPS = new MagicDGP::Point3DSet;
        for (int i = 0; i < pointNum; i++)
        {
            if (mpPointSet->GetPoint(i)->IsValid() == false)
            {
                continue;
            }
            MagicDGP::Point3D* pPoint = mpPointSet->GetPoint(i);
            MagicDGP::Point3D* pNewPoint = new MagicDGP::Point3D(pPoint->GetPosition(), pPoint->GetNormal());
            pNewPS->InsertPoint(pNewPoint);
        }
        delete mpPointSet;
        mpPointSet = pNewPS;
        /*int pointNum = mpPointSet->GetPointNumber();
        for (int i = 0; i < pointNum; i++)
        {
            mpPointSet->GetPoint(i)->SetValid(true);
        }
        int invalidNum = pointNum * proportion;
        int invalidIndex = 0;
        for (std::map<float, int>::reverse_iterator itr = mPSDensityMap.rbegin(); itr != mPSDensityMap.rend(); ++itr)
        {
            if (invalidIndex == invalidNum)
            {
                break;
            }
            mpPointSet->GetPoint(itr->second)->SetValid(false);
            invalidIndex++;
        }*/
        MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderOBJ", "SimplePoint", mpPointSet);
    }

    bool PointSetViewer::FilterMesh3D()
    {
        MagicDGP::Filter::SimpleMeshSmooth(mpMesh);
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderOBJ", "MyCookTorrance", mpMesh);
        return true;
    }
}
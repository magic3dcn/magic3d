#include "MeshShopApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../DGP/Consolidation.h"

namespace MagicApp
{
    MeshShopApp::MeshShopApp() :
        mpMesh(NULL),
        mpLightMesh(NULL),
        mMouseMode(MM_View)
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
        ModelViewer();
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
        if (mMouseMode == MM_View)
        {
            mViewTool.MouseMoved(arg);
        }
        else if (mMouseMode == MM_Pick_Rectangle || mMouseMode == MM_Pick_Cycle)
        {
            mPickTool.MouseMoved(arg);
        }

        return true;
    }

    bool MeshShopApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_View)
        {
            mViewTool.MousePressed(arg);
        }
        else if (mMouseMode == MM_Pick_Rectangle)
        {
            mPickTool.SetPickParameter(MagicTool::PM_Rectangle, mpLightMesh, NULL, NULL);
            mPickTool.MousePressed(arg);
        }
        else if (mMouseMode == MM_Pick_Cycle)
        {
            mPickTool.SetPickParameter(MagicTool::PM_Cycle, mpLightMesh, NULL, NULL);
            mPickTool.MousePressed(arg);
        }

        return true;
    }

    bool MeshShopApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_Pick_Rectangle || mMouseMode == MM_Pick_Cycle)
        {
            mPickTool.MouseReleased(arg);
            std::vector<int> pickIndex;
            mPickTool.GetPickLightMeshIndex(pickIndex);
            MagicDGP::Vector3 pickColor(0.663, 0.506, 0.459);
            for (std::vector<int>::iterator piIter = pickIndex.begin(); piIter != pickIndex.end(); ++piIter)
            {
                mPickIndexSet.insert(*piIter);
                mpLightMesh->GetVertex(*piIter)->SetColor(pickColor);
            }
            UpdateMeshRendering();
        }

        return  true;
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
        if (MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasSceneNode("ModelNode"))
        {
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->resetToInitialState();
        }
        ClearSceneData();
    }

    void MeshShopApp::UpdateMeshRendering()
    {
        //MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderMesh", "MyCookTorrance", mpMesh);
        MagicCore::RenderSystem::GetSingleton()->RenderLightMesh3D("RenderMesh", "MyCookTorrance", mpLightMesh);
    }

    void MeshShopApp::ClearSceneData()
    {
        mPickIndexSet.clear();
    }

    /*bool MeshShopApp::OpenMesh(int& vertNum)
    {
        std::string fileName;
        char filterName[] = "OBJ Files(*.obj)\0*.obj\0STL Files(*.stl)\0*.stl\0OFF Files(*.off)\0*.off\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            MagicDGP::Mesh3D* pMesh = MagicDGP::Parser::ParseMesh3D(fileName);
            if (pMesh != NULL)
            {
                if (pMesh->GetVertexNumber() > 0)
                {
                    mDefaultColor = pMesh->GetVertex(0)->GetColor();
                }
                vertNum = pMesh->GetVertexNumber();
                pMesh->UnifyPosition(2.0);
                pMesh->UpdateNormal();
                if (mpMesh != NULL)
                {
                    delete mpMesh;
                }
                mpMesh = pMesh;
                UpdateMeshRendering();
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
    }*/

    bool MeshShopApp::OpenMesh(int& vertNum)
    {
        std::string fileName;
        char filterName[] = "OBJ Files(*.obj)\0*.obj\0STL Files(*.stl)\0*.stl\0OFF Files(*.off)\0*.off\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            MagicDGP::LightMesh3D* pLightMesh = MagicDGP::Parser::ParseLightMesh3D(fileName);
            if (pLightMesh != NULL)
            {
                if (pLightMesh->GetVertexNumber() > 0)
                {
                    mDefaultColor = pLightMesh->GetVertex(0)->GetColor();
                }
                vertNum = pLightMesh->GetVertexNumber();
                pLightMesh->UnifyPosition(2.0);
                pLightMesh->UpdateNormal();
                if (mpLightMesh != NULL)
                {
                    delete mpLightMesh;
                }
                mpLightMesh = pLightMesh;
                UpdateMeshRendering();
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

    /*void MeshShopApp::SaveMesh()
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
    }*/

    void MeshShopApp::SaveMesh()
    {
        if (mpLightMesh != NULL)
        {
            std::string fileName;
            char filterName[] = "Support format(*.obj, *.stl, *.off)\0*.*\0";
            if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
            {
                MagicDGP::Parser::ExportLightMesh3D(fileName, mpLightMesh);
            }
        }
    }

    //void MeshShopApp::SmoothMesh()
    //{
    //    //MagicDGP::Consolidation::MeanCurvatureFlowFairing(mpMesh);
    //    MagicDGP::Consolidation::SimpleMeshSmooth(mpMesh);
    //    UpdateMeshRendering();
    //}

    void MeshShopApp::SmoothMesh()
    {
        MagicDGP::Consolidation::SimpleMeshSmooth(mpLightMesh);
        UpdateMeshRendering();
    }

    /*void MeshShopApp::RemoveOutlier()
    {
        MagicDGP::Mesh3D* pNewMesh = MagicDGP::Consolidation::RemoveSmallMeshPatch(mpMesh, 0.1);
        if (pNewMesh != NULL)
        {
            delete mpMesh;
            mpMesh = pNewMesh;
            UpdateMeshRendering();
        }
        else
        {
            DebugLog << "MeshShopApp::RemoveOutlier:: failed" << std::endl;
        }
    }*/

    void MeshShopApp::RemoveOutlier()
    {
        MagicDGP::LightMesh3D* pNewMesh = MagicDGP::Consolidation::RemoveSmallMeshPatch(mpLightMesh, 0.1);
        if (pNewMesh != NULL)
        {
            delete mpLightMesh;
            mpLightMesh = pNewMesh;
            UpdateMeshRendering();
        }
        else
        {
            DebugLog << "MeshShopApp::RemoveOutlier:: failed" << std::endl;
        }
    }

    void MeshShopApp::AddNoise()
    {
        int vertNum = mpLightMesh->GetVertexNumber();
        int maxNum = 100;
        MagicDGP::Real epsilon = 0.001;
        for (int vid = 0; vid < vertNum; vid++)
        {
            int randNum = rand();
            randNum = randNum % (2 * maxNum) - maxNum;
            MagicDGP::Real scale = MagicDGP::Real(randNum) / maxNum * epsilon;
            MagicDGP::Vector3 newPos = mpLightMesh->GetVertex(vid)->GetPosition() + mpLightMesh->GetVertex(vid)->GetNormal() * scale;
            mpLightMesh->GetVertex(vid)->SetPosition(newPos);
        }
        mpLightMesh->UpdateNormal();
        UpdateMeshRendering();
    }

    void MeshShopApp::RectangleSelect()
    {
        mMouseMode = MM_Pick_Rectangle;
    }

    void MeshShopApp::CycleSelect()
    {
        mMouseMode = MM_Pick_Cycle;
    }

    void MeshShopApp::ClearSelect()
    {
        for (std::set<int>::iterator pickItr = mPickIndexSet.begin(); pickItr != mPickIndexSet.end(); pickItr++)
        {
            mpMesh->GetVertex(*pickItr)->SetColor(mDefaultColor);
        }
        mPickIndexSet.clear();
        UpdateMeshRendering();
    }

    void MeshShopApp::DeleteSelcetVertex()
    {
        if (mpMesh != NULL && mPickIndexSet.size() > 0)
        {
            for (std::set<int>::iterator pickItr = mPickIndexSet.begin(); pickItr != mPickIndexSet.end(); ++pickItr)
            {
                mpMesh->GetVertex(*pickItr)->SetValid(false);
            }
            //Update to new mesh
            MagicDGP::Mesh3D* pNewMesh = new MagicDGP::Mesh3D;
            int vertNum = mpMesh->GetVertexNumber();
            std::vector<bool> visitFlag(vertNum, 0);
            std::map<int, int> vertIndexMap;
            int faceNum = mpMesh->GetFaceNumber();
            for (int fid = 0; fid < faceNum; fid++)
            {
                MagicDGP::Edge3D* pEdge = mpMesh->GetFace(fid)->GetEdge();
                MagicDGP::Vertex3D* pVert[3];
                pVert[0] = pEdge->GetVertex();
                if (pVert[0]->IsValid() == false)
                {
                    continue;
                }
                pVert[1] = pEdge->GetNext()->GetVertex();
                if (pVert[1]->IsValid() == false)
                {
                    continue;
                }
                pVert[2] = pEdge->GetPre()->GetVertex();
                if (pVert[2]->IsValid() == false)
                {
                    continue;
                }
                std::vector<MagicDGP::Vertex3D* > vertList(3);
                for (int k = 0; k < 3; k++)
                {
                    if (visitFlag.at(pVert[k]->GetId()) == true)
                    {
                        vertList.at(k) = pNewMesh->GetVertex(vertIndexMap[pVert[k]->GetId()]);
                    }
                    else
                    {
                        visitFlag.at(pVert[k]->GetId()) = true;
                        vertList.at(k) = pNewMesh->InsertVertex(pVert[k]->GetPosition());
                        vertIndexMap[pVert[k]->GetId()] = pNewMesh->GetVertexNumber() - 1;
                    }
                }
                pNewMesh->InsertFace(vertList);
            }
            pNewMesh->UpdateNormal();
            delete mpMesh;
            mpMesh = pNewMesh;
            ClearSceneData();
            UpdateMeshRendering();
        }
    }

    void MeshShopApp::ModelViewer()
    {
        mMouseMode = MM_View;
    }

    void MeshShopApp::SetupFromMeshInput(MagicDGP::Mesh3D* pMesh)
    {
        if (mpMesh != NULL)
        {
            delete mpMesh;
        }
        mpMesh = pMesh;
        UpdateMeshRendering();
        ClearSceneData();
        mUI.SetupFromMeshInput(mpMesh->GetVertexNumber());
    }
}
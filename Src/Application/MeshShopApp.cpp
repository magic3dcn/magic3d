#include "stdafx.h"
#include "MeshShopApp.h"
#include "../Tool/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../DGP/Consolidation.h"
//#include "../Common/MagicOgre.h"

namespace MagicApp
{
    MeshShopApp::MeshShopApp() :
        mpLightMesh(NULL),
        mMouseMode(MM_View),
        mPickIgnoreBack(false)
    {
    }

    MeshShopApp::~MeshShopApp()
    {
        if (mpLightMesh != NULL)
        {
            delete mpLightMesh;
            mpLightMesh = NULL;
        }
    }

    bool MeshShopApp::Enter(void)
    {
        InfoLog << "Enter MeshShopApp" << std::endl; 
        mUI.Setup();
        SetupScene();
        ModelViewer();
        mPickIgnoreBack = false;
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

    bool MeshShopApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_View)
        {
            mViewTool.MousePressed(arg.state.X.abs, arg.state.Y.abs);
        }
        else if (mMouseMode == MM_Pick_Rectangle)
        {
            mPickTool.SetPickParameter(MagicDGP::PM_Rectangle, mPickIgnoreBack, mpLightMesh, NULL, NULL);
            mPickTool.MousePressed(arg.state.X.abs, arg.state.Y.abs);
        }
        else if (mMouseMode == MM_Pick_Cycle)
        {
            mPickTool.SetPickParameter(MagicDGP::PM_Cycle, mPickIgnoreBack, mpLightMesh, NULL, NULL);
            mPickTool.MousePressed(arg.state.X.abs, arg.state.Y.abs);
        }
        return true;
    }

    bool MeshShopApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_Pick_Rectangle || mMouseMode == MM_Pick_Cycle)
        {
            mPickTool.MouseReleased(arg.state.X.abs, arg.state.Y.abs);
            std::vector<int> pickIndex;
            mPickTool.GetPickLightMeshIndex(pickIndex);
            MagicMath::Vector3 pickColor(0.663, 0.506, 0.459);
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
        if (arg.key == OIS::KC_V && mpLightMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_POINTS);
        }
        else if (arg.key == OIS::KC_E && mpLightMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_WIREFRAME);
        }
        else if (arg.key == OIS::KC_F && mpLightMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_SOLID);
        }
        else if (arg.key == OIS::KC_D && mpLightMesh !=NULL)
        {
            ExtractDepthDataTest();
        }
        else if (arg.key == OIS::KC_O && mpLightMesh !=NULL)
        {
            Ogre::Camera* pOrthCam = MagicCore::RenderSystem::GetSingleton()->GetMainCamera();
            pOrthCam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
            pOrthCam->setOrthoWindow(3, 3);
            pOrthCam->setPosition(0, 0, 3);
            pOrthCam->lookAt(0, 0, 0);
            pOrthCam->setNearClipDistance(0.5);
            pOrthCam->setFarClipDistance(5);
        }
        else if (arg.key == OIS::KC_P && mpLightMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
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
        MagicCore::RenderSystem::GetSingleton()->RenderLightMesh3D("RenderMesh", "MyCookTorrance", mpLightMesh);
        //MagicCore::RenderSystem::GetSingleton()->RenderLightMesh3D("RenderMesh", "Depth", mpLightMesh);
    }

    void MeshShopApp::ClearSceneData()
    {
        mPickIndexSet.clear();
    }

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

    void MeshShopApp::SmoothMesh()
    {
        MagicDGP::Consolidation::SimpleMeshSmooth(mpLightMesh);
        UpdateMeshRendering();
    }

    void MeshShopApp::RemoveOutlier()
    {
        MagicDGP::LightMesh3D* pNewMesh = MagicDGP::Consolidation::RemoveSmallMeshPatch(mpLightMesh, 0.1);
        if (pNewMesh != NULL)
        {
            delete mpLightMesh;
            mpLightMesh = pNewMesh;
            mpLightMesh->UnifyPosition(2);
            mpLightMesh->UpdateNormal();
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
        double epsilon = 0.001;
        for (int vid = 0; vid < vertNum; vid++)
        {
            int randNum = rand();
            randNum = randNum % (2 * maxNum) - maxNum;
            double scale = double(randNum) / maxNum * epsilon;
            MagicMath::Vector3 newPos = mpLightMesh->GetVertex(vid)->GetPosition() + mpLightMesh->GetVertex(vid)->GetNormal() * scale;
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
            mpLightMesh->GetVertex(*pickItr)->SetColor(mDefaultColor);
        }
        mPickIndexSet.clear();
        UpdateMeshRendering();
    }

    void MeshShopApp::DeleteSelcetVertex()
    {
        if (mpLightMesh != NULL && mPickIndexSet.size() > 0)
        {
            for (std::set<int>::iterator pickItr = mPickIndexSet.begin(); pickItr != mPickIndexSet.end(); ++pickItr)
            {
                mpLightMesh->GetVertex(*pickItr)->SetValid(false);
            }
            //Update to new mesh
            MagicDGP::LightMesh3D* pNewMesh = new MagicDGP::LightMesh3D;
            int vertNum = mpLightMesh->GetVertexNumber();
            std::vector<bool> visitFlag(vertNum, 0);
            std::map<int, int> vertIndexMap;
            int faceNum = mpLightMesh->GetFaceNumber();
            for (int fid = 0; fid < faceNum; fid++)
            {
                MagicDGP::FaceIndex faceIdx = mpLightMesh->GetFace(fid);
                if (mpLightMesh->GetVertex(faceIdx.mIndex[0])->IsValid() == false)
                {
                    continue;
                }
                if (mpLightMesh->GetVertex(faceIdx.mIndex[1])->IsValid() == false)
                {
                    continue;
                }
                if (mpLightMesh->GetVertex(faceIdx.mIndex[2])->IsValid() == false)
                {
                    continue;
                }
                MagicDGP::FaceIndex newFaceIdx;
                for (int k = 0; k < 3; k++)
                {
                    if (visitFlag.at(faceIdx.mIndex[k]) == true)
                    {
                        newFaceIdx.mIndex[k] = vertIndexMap[faceIdx.mIndex[k]];
                    }
                    else
                    {
                        visitFlag.at(faceIdx.mIndex[k]) = true;
                        newFaceIdx.mIndex[k] = pNewMesh->InsertVertex(mpLightMesh->GetVertex(faceIdx.mIndex[k])->GetPosition())->GetId();
                        vertIndexMap[faceIdx.mIndex[k]] = newFaceIdx.mIndex[k];
                    }
                }
                pNewMesh->InsertFace(newFaceIdx);
            }
            pNewMesh->UnifyPosition(2);
            pNewMesh->UpdateNormal();
            delete mpLightMesh;
            mpLightMesh = pNewMesh;
            ClearSceneData();
            UpdateMeshRendering();
        }
    }

    void MeshShopApp::SetPickIgnoreBack(bool ignore)
    {
        mPickIgnoreBack = ignore;
    }

    void MeshShopApp::ModelViewer()
    {
        mMouseMode = MM_View;
    }

    void MeshShopApp::SetupFromMeshInput(MagicDGP::LightMesh3D* pMesh)
    {
        if (mpLightMesh != NULL)
        {
            delete mpLightMesh;
        }
        mpLightMesh = pMesh;
        UpdateMeshRendering();
        ClearSceneData();
        mUI.SetupFromMeshInput(mpLightMesh->GetVertexNumber());
    }

    void MeshShopApp::ExtractDepthDataTest()
    {
        DebugLog << "ExtractDepthDataTest" << std::endl;
        Ogre::TexturePtr depthTex = Ogre::TextureManager::getSingleton().createManual(  
            "DepthTexture",      // name   
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,  
            Ogre::TEX_TYPE_2D,   // type   
            512,  // width   
            512,  // height   
            0,                   // number of mipmaps   
            //Ogre::PF_B8G8R8A8,   // pixel format
            Ogre::PF_FLOAT32_R,
            Ogre::TU_RENDERTARGET
            ); 
        Ogre::RenderTarget* pTarget = depthTex->getBuffer()->getRenderTarget();
        Ogre::Camera* pOrthCam = MagicCore::RenderSystem::GetSingleton()->GetMainCamera();
        pOrthCam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
        pOrthCam->setOrthoWindow(3, 3);
        pOrthCam->setPosition(0, 0, 3);
        pOrthCam->lookAt(0, 0, 0);
        pOrthCam->setAspectRatio(1.0);
        pOrthCam->setNearClipDistance(0.5);
        pOrthCam->setFarClipDistance(5);
        Ogre::Viewport* pViewport = pTarget->addViewport(pOrthCam);
        pViewport->setDimensions(0, 0, 1, 1);

        DebugLog << "Render light mesh" << std::endl;
        MagicCore::RenderSystem::GetSingleton()->RenderLightMesh3D("RenderMesh", "Depth", mpLightMesh);
        DebugLog << "RenderSystem Update" << std::endl;
        MagicCore::RenderSystem::GetSingleton()->Update();

        DebugLog << "Convert image" << std::endl;
        Ogre::Image img;
        depthTex->convertToImage(img);
        MagicDGP::Point3DSet* pPS = new MagicDGP::Point3DSet;
        for(int y = 0; y < 512; y++)  
        {  
            for(int x = 0; x < 512; x++)  
            {
                MagicMath::Vector3 pos(x / 512.0, y / 512.0, 0);
                pos[2] = (img.getColourAt(x, y, 0))[1];
                MagicDGP::Point3D* pPoint = new MagicDGP::Point3D(pos);
                pPS->InsertPoint(pPoint);
            }
        }
        MagicDGP::Parser::ExportPointSet("depth.obj", pPS);
        img.save("depth.bmp");
        DebugLog << "End" << std::endl;
    }
}
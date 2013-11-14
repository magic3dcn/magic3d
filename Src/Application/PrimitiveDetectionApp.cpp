#include "PrimitiveDetectionApp.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"
#include "../Common/RenderSystem.h"
#include "../DGP/PrimitiveDetection.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../DGP/Curvature.h"
#include "../DGP/Filter.h"

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
        else if (arg.key == OIS::KC_G && mpMesh != NULL)
        {
            CalMeshCurvature();
        }
        else if (arg.key == OIS::KC_S && mpMesh != NULL)
        {
            FilterMesh3D();
        }
        else if (arg.key == OIS::KC_N && mpMesh != NULL)
        {
            CalNormalDeviation();
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
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetToInitialState();
    }

    bool PrimitiveDetectionApp::ImportMesh3D()
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
        //MagicDGP::PrimitiveDetection::Primitive2DDetection(mpMesh, res);
        MagicDGP::PrimitiveDetection::Primitive2DDetection_WithoutRefitting(mpMesh, res);
        int vertNum = mpMesh->GetVertexNumber();
        for (int i = 0; i < vertNum; i++)
        {
            float cv = res.at(i) * 0.2f;
            MagicDGP::Vector3 color = MagicCore::ToolKit::GetSingleton()->ColorCoding(cv);
            mpMesh->GetVertex(i)->SetColor(color);
        }
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
    }

    void PrimitiveDetectionApp::CalMeshCurvature()
    {
        mpMesh->CalculateBBox();
        MagicDGP::Vector3 bboxMin, bboxMax;
        mpMesh->GetBBox(bboxMin, bboxMax);
        MagicDGP::Real bboxSize = (bboxMax - bboxMin).Length();
        MagicDGP::Real bboxArea = bboxSize * bboxSize;
        mpMesh->CalculateFaceArea();

        int filterNum = 3;
        int vertNum = mpMesh->GetVertexNumber();
        std::vector<MagicDGP::Real> gaussianCurvList(vertNum, 0);
        for (int filterIdx = 0; filterIdx < filterNum; filterIdx++)
        {
            MagicDGP::Filter::SimpleMeshSmooth(mpMesh);
            std::vector<MagicDGP::Real> localGaussianCurvList;
            MagicDGP::Curvature::CalGaussianCurvature(mpMesh, localGaussianCurvList);
            for (int vid = 0; vid < vertNum; vid++)
            {
                gaussianCurvList.at(vid) += localGaussianCurvList.at(vid);
            }
        }
        for (int vid = 0; vid < vertNum; vid++)
        {
            gaussianCurvList.at(vid) /= filterNum;
        }
        //std::vector<MagicDGP::Real> gaussianCurvList;
        //MagicDGP::Curvature::CalGaussianCurvature(mpMesh, gaussianCurvList);
        //int vertNum = mpMesh->GetVertexNumber();

        for (int vid = 0; vid < vertNum; vid++)
        {
            MagicDGP::Vertex3D* pVert = mpMesh->GetVertex(vid);
            MagicDGP::Edge3D* pEdge = pVert->GetEdge();
            MagicDGP::Real area = 0;
            do
            {
                MagicDGP::Face3D* pFace = pEdge->GetFace();
                if (pFace != NULL)
                {
                    area += pFace->GetArea();
                }
                pEdge = pEdge->GetPair()->GetNext();
            } while (pEdge != NULL && pEdge != pVert->GetEdge());
            area /= bboxArea;
            float cv = gaussianCurvList.at(vid);
            if (area > 1.0e-10)
            {
                cv /= area;    
            }
            cv = 0.6 + cv * 0.05;
            //if (cv > 0.7)
            //{
            //    cv = 0;
            //}
            //else
            //{
            //    cv = 0.6;
            //}
            MagicLog << "Gaussian: " << cv << std::endl;
            MagicDGP::Vector3 color = MagicCore::ToolKit::GetSingleton()->ColorCoding(cv);
            mpMesh->GetVertex(vid)->SetColor(color);
        }
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
    }

    void PrimitiveDetectionApp::CalNormalDeviation()
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();

        int vertNum = mpMesh->GetVertexNumber();
        for (int vid = 0; vid < vertNum; vid++)
        {
            std::vector<int> neighborList;
            neighborList.reserve(10);
            MagicDGP::Vertex3D* pVert = mpMesh->GetVertex(vid);
            MagicDGP::Edge3D* pEdge = pVert->GetEdge();
            do
            {
                if (pEdge == NULL)
                {
                    break;
                }
                neighborList.push_back(pEdge->GetVertex()->GetId());
                pEdge = pEdge->GetPair()->GetNext();
            } while (pEdge != pVert->GetEdge());

            MagicDGP::Vector3 normal = mpMesh->GetVertex(vid)->GetNormal();
            MagicDGP::Real nDev = 0;
            for (std::vector<int>::iterator neigItr = neighborList.begin(); neigItr != neighborList.end(); ++neigItr)
            {
                nDev += (normal - (mpMesh->GetVertex(*neigItr)->GetNormal())).Length();
            }
            if (neighborList.size() > 0)
            {
                nDev /= neighborList.size();
            }
            nDev = nDev * 8 + 0.2;
            if (nDev > 1)
            {
                nDev = 1.2;
            }
            else
            {
                nDev = 0.4;
            }
            //MagicLog << "Normal Deviation: " << nDev << " neigbor size: " << neighborList.size() << std::endl;
            MagicDGP::Vector3 color = MagicCore::ToolKit::GetSingleton()->ColorCoding(nDev);
            mpMesh->GetVertex(vid)->SetColor(color);
        }
        MagicLog << "CalNormalDeviation: total time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
    }

    void PrimitiveDetectionApp::FilterMesh3D()
    {
        MagicDGP::Filter::SimpleMeshSmooth(mpMesh);
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
    }
}
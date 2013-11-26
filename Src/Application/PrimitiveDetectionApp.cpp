#include "PrimitiveDetectionApp.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../DGP/Curvature.h"
#include "../DGP/Filter.h"
#include "../Tool/PickPointTool.h"
#include "../DGP/HomoMatrix4.h"

namespace MagicApp
{
    PrimitiveDetectionApp::PrimitiveDetectionApp() : 
        mpMesh(NULL),
        mIsPickingMode(false)
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
        InfoLog << "Enter PrimitiveDetectionApp" << std::endl;
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
        if (mIsPickingMode == false)
        {
            mViewTool.MouseMoved(arg);
        }

        return true;
    }

    bool PrimitiveDetectionApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mIsPickingMode == false)
        {
            mViewTool.MousePressed(arg);
        }

        return true;
    }

    bool PrimitiveDetectionApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mIsPickingMode)
        {
            if (mpMesh != NULL)
            {
                MagicDGP::Vector2 mousePos(arg.state.X.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getWidth() - 1.0, 
                    1.0 - arg.state.Y.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getHeight());
                int pickIndex = MagicTool::PickPointTool::PickMeshVertex(mpMesh, mousePos);
                if (pickIndex != -1)
                {
                    PrimitiveSelection(pickIndex);
                    mpMesh->GetVertex(pickIndex)->SetColor(MagicDGP::Vector3(0, 0, 0));
                    MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
                }
            }
            
        }
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
        else if (arg.key == OIS::KC_A && mpMesh != NULL)
        {
            SampleVertex();
        }
        else if (arg.key == OIS::KC_P && mpMesh != NULL)
        {
            mIsPickingMode = !mIsPickingMode;
        }
        return true;
    }

    void PrimitiveDetectionApp::SetupScene(void)
    {
        InfoLog << "PrimitiveDetectionApp::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light*  sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(10, 10, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }
     
    void PrimitiveDetectionApp::ShutdownScene(void)
    {
        InfoLog << "PrimitiveDetectionApp::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        if (mpMesh != NULL)
        {
            delete mpMesh;
            mpMesh = NULL;
        }
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("Mesh3D");
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("Primitive");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetToInitialState();
    }

    bool PrimitiveDetectionApp::ImportMesh3D()
    {
        std::string fileName;
        char filterName[] = "OBJ Files(*.obj)\0*.obj\0";
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
                MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
                MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("Primitive");
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
        MagicDGP::PrimitiveDetection::Primitive2DDetectionEnhance(mpMesh, res);
        int vertNum = mpMesh->GetVertexNumber();
        for (int i = 0; i < vertNum; i++)
        {
            float cv = res.at(i) * 0.2f;
            MagicDGP::Vector3 color = MagicCore::ToolKit::ColorCoding(cv);
            mpMesh->GetVertex(i)->SetColor(color);
        }
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
    }

    void PrimitiveDetectionApp::PrimitiveSelection(int sampleId)
    {
        int vertNum = mpMesh->GetVertexNumber();
        std::vector<int> res;
        MagicDGP::ShapeCandidate* pCand = MagicDGP::PrimitiveDetection::Primitive2DSelectionByVertex(mpMesh, sampleId, res);
        for (int i = 0; i < vertNum; i++)
        {
            if (res.at(i) == MagicDGP::PrimitiveType::None)
            {
                mpMesh->GetVertex(i)->SetColor(MagicDGP::Vector3(0.9, 0.9, 0.9));
            }
            else
            {
                float cv = res.at(i) * 0.2f;
                MagicDGP::Vector3 color = MagicCore::ToolKit::ColorCoding(cv);
                mpMesh->GetVertex(i)->SetColor(color);
            }
        }
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
        DrawPrimitive(pCand);
        if (pCand != NULL)
        {
            delete pCand;
        }
    }

    void PrimitiveDetectionApp::DrawPrimitive(MagicDGP::ShapeCandidate* pCand)
    {
        if (pCand != NULL)
        {
            MagicDGP::PrimitiveType candType = pCand->GetType();
            if (candType == MagicDGP::PrimitiveType::Plane)
            {
                MagicDGP::PlaneCandidate* planeCand = dynamic_cast<MagicDGP::PlaneCandidate* >(pCand);
                MagicDGP::Vector3 centerPos = planeCand->mCenter;
                MagicDGP::Vector3 normal = planeCand->mNormal;
                MagicDGP::HomoMatrix4 rotateMat;
                rotateMat.GenerateVectorToVectorRotation(MagicDGP::Vector3(0, 0, 1), normal);
                MagicDGP::HomoMatrix4 translateMat;
                translateMat.GenerateTranslation(centerPos);
                MagicDGP::HomoMatrix4 totalMat = translateMat * rotateMat;
                MagicDGP::Mesh3D* planeMesh = MagicDGP::Parser::ParseMesh3D("../../Media/Model/plane.obj");
                if (planeMesh != NULL)
                {
                    int vertNum = planeMesh->GetVertexNumber();
                    for (int vid = 0; vid < vertNum; vid++)
                    {
                        MagicDGP::Vertex3D* pVert = planeMesh->GetVertex(vid);
                        MagicDGP::Vector3 pos = pVert->GetPosition();
                        pos = totalMat.TransformPoint(pos);
                        pVert->SetPosition(pos);
                    }
                    planeMesh->UpdateNormal();
                    MagicCore::RenderSystem::GetSingleton()->RenderBlendMesh3D("Primitive", "MyCookTorranceBlend", planeMesh, 0.6);
                }
            }
            else if (candType == MagicDGP::PrimitiveType::Sphere)
            {
                MagicDGP::SphereCandidate* sphereCand = dynamic_cast<MagicDGP::SphereCandidate* >(pCand);
                MagicDGP::Vector3 centerPos = sphereCand->mCenter;
                MagicDGP::Real radius = sphereCand->mRadius;
                MagicDGP::HomoMatrix4 translateMat;
                translateMat.GenerateTranslation(centerPos);
                MagicDGP::Mesh3D* sphereMesh = MagicDGP::Parser::ParseMesh3D("../../Media/Model/sphere.obj");
                if (sphereCand != NULL)
                {
                    int vertNum = sphereMesh->GetVertexNumber();
                    for (int vid = 0; vid < vertNum; vid++)
                    {
                        MagicDGP::Vertex3D* pVert = sphereMesh->GetVertex(vid);
                        MagicDGP::Vector3 pos = pVert->GetPosition();
                        pos *= radius;
                        pos = translateMat.TransformPoint(pos);
                        pVert->SetPosition(pos);
                    }
                    sphereMesh->UpdateNormal();
                    MagicCore::RenderSystem::GetSingleton()->RenderBlendMesh3D("Primitive", "MyCookTorranceBlend", sphereMesh, 0.8);
                }
            }
            else if (candType == MagicDGP::PrimitiveType::Cylinder)
            {
                MagicDGP::CylinderCandidate* cylinderCand = dynamic_cast<MagicDGP::CylinderCandidate* >(pCand);
                MagicDGP::Vector3 centerPos = cylinderCand->mCenter;
                MagicDGP::Vector3 direction = cylinderCand->mDir;
                MagicDGP::Real radius = cylinderCand->mRadius;
                MagicDGP::HomoMatrix4 rotateMat;
                rotateMat.GenerateVectorToVectorRotation(MagicDGP::Vector3(0, 0, 1), direction);
                MagicDGP::HomoMatrix4 translateMat;
                translateMat.GenerateTranslation(centerPos);
                MagicDGP::HomoMatrix4 totalMat = translateMat * rotateMat;
                MagicDGP::Mesh3D* cylinderMesh = MagicDGP::Parser::ParseMesh3D("../../Media/Model/cylinder.obj");
                if (cylinderMesh != NULL)
                {
                    int vertNum = cylinderMesh->GetVertexNumber();
                    for (int vid = 0; vid < vertNum; vid++)
                    {
                        MagicDGP::Vertex3D* pVert = cylinderMesh->GetVertex(vid);
                        MagicDGP::Vector3 pos = pVert->GetPosition();
                        pos *= radius;
                        pos = totalMat.TransformPoint(pos);
                        pVert->SetPosition(pos);
                    }
                    cylinderMesh->UpdateNormal();
                    MagicCore::RenderSystem::GetSingleton()->RenderBlendMesh3D("Primitive", "MyCookTorranceBlend", cylinderMesh, 0.6);
                }
            }
            else if (candType == MagicDGP::PrimitiveType::Cone)
            {
                MagicDGP::ConeCandidate* coneCand = dynamic_cast<MagicDGP::ConeCandidate* >(pCand);
                MagicDGP::Vector3 apex = coneCand->mApex;
                MagicDGP::Vector3 direction = coneCand->mDir;
                MagicDGP::Real angle = coneCand->mAngle;
                MagicDGP::Real radius = tan(angle);
                MagicDGP::HomoMatrix4 rotateMat;
                MagicDGP::Vector3 zAxis(0, 0, 1);
                if ((zAxis.CrossProduct(direction)).LengthSquared() < MagicDGP::Epsilon &&
                    zAxis * direction < 0)
                {
                    MagicDGP::HomoMatrix4 rotateTemp0, rotateTemp1;
                    rotateTemp0.GenerateVectorToVectorRotation(zAxis, MagicDGP::Vector3(0, 1, 0));
                    rotateTemp1.GenerateVectorToVectorRotation(MagicDGP::Vector3(0, 1, 0), direction);
                    rotateMat = rotateTemp1 * rotateTemp0;
                }
                else
                {
                    rotateMat.GenerateVectorToVectorRotation(MagicDGP::Vector3(0, 0, 1), direction);
                }
                MagicDGP::HomoMatrix4 translateMat;
                translateMat.GenerateTranslation(apex);
                MagicDGP::HomoMatrix4 totalMat = translateMat * rotateMat;
                MagicDGP::Mesh3D* coneMesh = MagicDGP::Parser::ParseMesh3D("../../Media/Model/cone.obj");
                if (coneMesh != NULL)
                {
                    int vertNum = coneMesh->GetVertexNumber();
                    for (int vid = 0; vid < vertNum; vid++)
                    {
                        MagicDGP::Vertex3D* pVert = coneMesh->GetVertex(vid);
                        MagicDGP::Vector3 pos = pVert->GetPosition();
                        pos[0] = pos[0] * radius;
                        pos[1] = pos[1] * radius;
                        pos = totalMat.TransformPoint(pos);
                        pVert->SetPosition(pos);
                    }
                    coneMesh->UpdateNormal();
                    MagicCore::RenderSystem::GetSingleton()->RenderBlendMesh3D("Primitive", "MyCookTorranceBlend", coneMesh, 0.6);
                }
            }
            else
            {
                MagicLog(MagicCore::LOGLEVEL_DEBUG) << "No Type" << std::endl;
            }
        }
        else
        {
            MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("Primitive");
        }
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
            MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Gaussian: " << cv << std::endl;
            MagicDGP::Vector3 color = MagicCore::ToolKit::ColorCoding(cv);
            mpMesh->GetVertex(vid)->SetColor(color);
        }
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
    }

    void PrimitiveDetectionApp::CalNormalDeviation()
    {
        float timeStart = MagicCore::ToolKit::GetTime();

        int vertNum = mpMesh->GetVertexNumber();
        MagicDGP::Real scale = 5;
        std::vector<MagicDGP::Real> norDev(vertNum);
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
                //nDev += (normal - (mpMesh->GetVertex(*neigItr)->GetNormal())).Length();
                MagicDGP::Real cosA = normal * (mpMesh->GetVertex(*neigItr)->GetNormal());
                cosA = cosA > 1 ? 1 : (cosA < -1 ? -1 : cosA);
                nDev += acos(cosA);
            }
            if (neighborList.size() > 0)
            {
                nDev /= neighborList.size();
            }
            norDev.at(vid) = nDev;
        }
        std::map<MagicDGP::Real, int> scoreMap;
        for (int vid = 0; vid < vertNum; vid++)
        {
            MagicDGP::Vertex3D* pVert = mpMesh->GetVertex(vid);
            MagicDGP::Edge3D* pEdge = pVert->GetEdge();
            MagicDGP::Real devGrad = 0;
            int neigNum = 0;
            do
            {
                if (pEdge == NULL)
                {
                    break;
                }
                devGrad += fabs(norDev.at(vid) - norDev.at(pEdge->GetVertex()->GetId()));
                neigNum++;
                pEdge = pEdge->GetPair()->GetNext();
            } while (pEdge != pVert->GetEdge());
            if (neigNum > 0)
            {
                devGrad /= neigNum;
            }
            devGrad = devGrad * scale + 0.2;
            /*if (devGrad > 0.5)
            {
                devGrad = 1.2;
            }
            else
            {
                devGrad = 0.4;
            }*/
            scoreMap[devGrad] = vid;
            MagicDGP::Vector3 color = MagicCore::ToolKit::ColorCoding(devGrad);
            mpMesh->GetVertex(vid)->SetColor(color);
        }
        int validNum = vertNum / 2;
        int validIndex = 0;
        MagicDGP::Vector3 color(77.0 / 255.0, 0, 153.0 / 255.0);
        for (std::map<MagicDGP::Real, int>::iterator itr = scoreMap.begin(); itr != scoreMap.end(); ++itr)
        {
            mpMesh->GetVertex(itr->second)->SetColor(color);
            validIndex++;
            if (validIndex == validNum)
            {
                break;
            }
        }
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "CalNormalDeviation: total time: " << MagicCore::ToolKit::GetTime() - timeStart << std::endl;
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
    }

    void PrimitiveDetectionApp::SampleVertex()
    {
        if (mpMesh != NULL)
        {
            std::vector<int> sampleIndex;
            MagicDGP::Filter::MeshVertexSampling(mpMesh, 200, sampleIndex);
            MagicDGP::Vector3 color = MagicCore::ToolKit::ColorCoding(0.2);
            for (int sid = 0; sid < sampleIndex.size(); sid++)
            {
                mpMesh->GetVertex(sampleIndex.at(sid))->SetColor(color);
            }
            MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
        }
    }

    void PrimitiveDetectionApp::FilterMesh3D()
    {
        MagicDGP::Filter::SimpleMeshSmooth(mpMesh);
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("Mesh3D", "MyCookTorrance", mpMesh);
    }
}
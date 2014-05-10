#include "stdafx.h"
#include "PickPointTool.h"
#include "../Common/RenderSystem.h"
//#include "../Common/MagicOgre.h"

namespace MagicTool
{
    PickPointTool::PickPointTool() :
        mPickMode(PM_Point),
        mIgnoreBack(true),
        mMousePos(0, 0),
        mpMesh(NULL),
        mpLightMesh(NULL),
        mpPointSet(NULL),
        mPickPressed(false)
    {
    }

    PickPointTool::~PickPointTool()
    {
    }

    int PickPointTool::PickMeshVertexByPoint(const MagicDGP::Mesh3D* pMesh, MagicMath::Vector2 mousePos, bool ignoreBack)
    {
        if (MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasSceneNode("ModelNode") == false)
        {
            return -1;
        }
        double pointSizeSquared = 0.01 * 0.01;
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        double minZ = 1.0e10;
        int pickIndex = -1;
        int vertNum = pMesh->GetVertexNumber();
        if (ignoreBack == true)
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vNor = pMesh->GetVertex(vid)->GetNormal();
                Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                ogreVNor = worldM * ogreVNor;
                if (ogreVNor.z > 0)
                {
                    MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                    Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                    ogreVPos = wvpM * ogreVPos;
                    MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                    if ((screenPos - mousePos).LengthSquared() < pointSizeSquared)
                    {
                        if (ogreVPos.z < minZ)
                        {
                            minZ = ogreVPos.z;
                            pickIndex = vid;
                        }
                    }
                }
            }
        }
        else
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                if ((screenPos - mousePos).LengthSquared() < pointSizeSquared)
                {
                    if (ogreVPos.z < minZ)
                    {
                        minZ = ogreVPos.z;
                        pickIndex = vid;
                    }
                }
            }
        }

        return pickIndex;
    }

    int PickPointTool::PickMeshVertexByPoint(const MagicDGP::LightMesh3D* pMesh, MagicMath::Vector2 mousePos, bool ignoreBack)
    {
        if (MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasSceneNode("ModelNode") == false)
        {
            return -1;
        }
        double pointSizeSquared = 0.01 * 0.01;
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        double minZ = 1.0e10;
        int pickIndex = -1;
        int vertNum = pMesh->GetVertexNumber();
        if (ignoreBack == true)
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vNor = pMesh->GetVertex(vid)->GetNormal();
                Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                ogreVNor = worldM * ogreVNor;
                if (ogreVNor.z > 0)
                {
                    MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                    Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                    ogreVPos = wvpM * ogreVPos;
                    MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                    if ((screenPos - mousePos).LengthSquared() < pointSizeSquared)
                    {
                        if (ogreVPos.z < minZ)
                        {
                            minZ = ogreVPos.z;
                            pickIndex = vid;
                        }
                    }
                }
            }
        }
        else
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                if ((screenPos - mousePos).LengthSquared() < pointSizeSquared)
                {
                    if (ogreVPos.z < minZ)
                    {
                        minZ = ogreVPos.z;
                        pickIndex = vid;
                    }
                }
            }
        }

        return pickIndex;
    }

    void PickPointTool::PickMeshVertexByRectangle(const MagicDGP::Mesh3D* pMesh, MagicMath::Vector2 pos0, 
            MagicMath::Vector2 pos1, std::vector<int>& pickIndex, bool ignoreBack)
    {
        pickIndex.clear();
        double minX = (pos0[0] < pos1[0]) ? pos0[0] : pos1[0];
        double maxX = (pos0[0] > pos1[0]) ? pos0[0] : pos1[0];
        double minY = (pos0[1] < pos1[1]) ? pos0[1] : pos1[1];
        double maxY = (pos0[1] > pos1[1]) ? pos0[1] : pos1[1];

        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;

        int vertNum = pMesh->GetVertexNumber();
        if (ignoreBack == true)
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vNor = pMesh->GetVertex(vid)->GetNormal();
                Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                ogreVNor = worldM * ogreVNor;
                if (ogreVNor.z > 0)
                {
                    MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                    Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                    ogreVPos = wvpM * ogreVPos;
                    if (ogreVPos.x > minX && ogreVPos.x < maxX && ogreVPos.y > minY && ogreVPos.y < maxY)
                    {
                        pickIndex.push_back(vid);
                    }
                }
            }
        }
        else
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                if (ogreVPos.x > minX && ogreVPos.x < maxX && ogreVPos.y > minY && ogreVPos.y < maxY)
                {
                    pickIndex.push_back(vid);
                }
            }
        }
    }

    void PickPointTool::PickMeshVertexByRectangle(const MagicDGP::LightMesh3D* pMesh, MagicMath::Vector2 pos0, 
            MagicMath::Vector2 pos1, std::vector<int>& pickIndex, bool ignoreBack)
    {
        pickIndex.clear();
        double minX = (pos0[0] < pos1[0]) ? pos0[0] : pos1[0];
        double maxX = (pos0[0] > pos1[0]) ? pos0[0] : pos1[0];
        double minY = (pos0[1] < pos1[1]) ? pos0[1] : pos1[1];
        double maxY = (pos0[1] > pos1[1]) ? pos0[1] : pos1[1];

        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;

        int vertNum = pMesh->GetVertexNumber();
        if (ignoreBack == true)
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vNor = pMesh->GetVertex(vid)->GetNormal();
                Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                ogreVNor = worldM * ogreVNor;
                if (ogreVNor.z > 0)
                {
                    MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                    Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                    ogreVPos = wvpM * ogreVPos;
                    if (ogreVPos.x > minX && ogreVPos.x < maxX && ogreVPos.y > minY && ogreVPos.y < maxY)
                    {
                        pickIndex.push_back(vid);
                    }
                }
            }
        }
        else
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                if (ogreVPos.x > minX && ogreVPos.x < maxX && ogreVPos.y > minY && ogreVPos.y < maxY)
                {
                    pickIndex.push_back(vid);
                }
            }
        }
    }

    void PickPointTool::PickMeshVertexByCycle(const MagicDGP::Mesh3D* pMesh, MagicMath::Vector2 centerPos, 
            double radius, std::vector<int>& pickIndex, bool ignoreBack)
    {
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        int vertNum = pMesh->GetVertexNumber();
        if (ignoreBack == true)
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vNor = pMesh->GetVertex(vid)->GetNormal();
                Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                ogreVNor = worldM * ogreVNor;
                if (ogreVNor.z > 0)
                {
                    MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                    Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                    ogreVPos = wvpM * ogreVPos;
                    MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                    if ((screenPos - centerPos).Length() < radius)
                    {
                        pickIndex.push_back(vid);
                    }
                }
            }
        }
        else
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                if ((screenPos - centerPos).Length() < radius)
                {
                    pickIndex.push_back(vid);
                }
            }
        }
    }

    void PickPointTool::PickMeshVertexByCycle(const MagicDGP::LightMesh3D* pMesh, MagicMath::Vector2 centerPos, 
            double radius, std::vector<int>& pickIndex, bool ignoreBack)
    {
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        int vertNum = pMesh->GetVertexNumber();
        if (ignoreBack == true)
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vNor = pMesh->GetVertex(vid)->GetNormal();
                Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                ogreVNor = worldM * ogreVNor;
                if (ogreVNor.z > 0)
                {
                    MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                    Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                    ogreVPos = wvpM * ogreVPos;
                    MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                    if ((screenPos - centerPos).Length() < radius)
                    {
                        pickIndex.push_back(vid);
                    }
                }
            }
        }
        else
        {
            for (int vid = 0; vid < vertNum; vid++)
            {
                MagicMath::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                if ((screenPos - centerPos).Length() < radius)
                {
                    pickIndex.push_back(vid);
                }
            }
        }
    }

    int PickPointTool::PickPointSetByPoint(const MagicDGP::Point3DSet* pPS, MagicMath::Vector2 mousePos, bool ignoreBack)
    {
        if (MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasSceneNode("ModelNode") == false)
        {
            return -1;
        }
        double pointSizeSquared = 0.01 * 0.01;
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        double minZ = 1.0e10;
        int pickIndex = -1;
        int pointNum = pPS->GetPointNumber();
        if (pPS->HasNormal() && ignoreBack)
        {
            for (int pid = 0; pid < pointNum; pid++)
            {
                MagicMath::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                MagicMath::Vector3 vNor = pPS->GetPoint(pid)->GetNormal();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                ogreVPos = wvpM * ogreVPos;
                MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                if ((screenPos - mousePos).LengthSquared() < pointSizeSquared)
                {
                    ogreVNor = worldM * ogreVNor;
                    if (ogreVNor.z > 0)
                    {
                        if (ogreVPos.z < minZ)
                        {
                            minZ = ogreVPos.z;
                            pickIndex = pid;
                        }
                    }
                }
            }
        }
        else
        {
            for (int pid = 0; pid < pointNum; pid++)
            {
                MagicMath::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                if ((screenPos - mousePos).LengthSquared() < pointSizeSquared)
                {
                    if (ogreVPos.z < minZ)
                    {
                        minZ = ogreVPos.z;
                        pickIndex = pid;
                    }
                }
            }
        }

        return pickIndex;
    }

    void PickPointTool::PickPointSetByRectangle(const MagicDGP::Point3DSet* pPS, MagicMath::Vector2 pos0,
            MagicMath::Vector2 pos1, std::vector<int>& pickIndex, bool ignoreBack)
    {
        pickIndex.clear();
        double minX = (pos0[0] < pos1[0]) ? pos0[0] : pos1[0];
		double maxX = (pos0[0] > pos1[0]) ? pos0[0] : pos1[0];
		double minY = (pos0[1] < pos1[1]) ? pos0[1] : pos1[1];
		double maxY = (pos0[1] > pos1[1]) ? pos0[1] : pos1[1];

        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        if (pPS->HasNormal() && ignoreBack)
        {
            int pointNum = pPS->GetPointNumber();
            for (int pid = 0; pid < pointNum; pid++)
            {
                MagicMath::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                if (ogreVPos.x > minX && ogreVPos.x < maxX && ogreVPos.y > minY && ogreVPos.y < maxY)
                {
                    MagicMath::Vector3 vNor = pPS->GetPoint(pid)->GetNormal();
                    Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                    ogreVNor = worldM * ogreVNor;
                    if (ogreVNor.z > 0)
                    {
                        pickIndex.push_back(pid);
                    }
                }
            }
        }
        else
        {
            int pointNum = pPS->GetPointNumber();
            for (int pid = 0; pid < pointNum; pid++)
            {
                MagicMath::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                if (ogreVPos.x > minX && ogreVPos.x < maxX && ogreVPos.y > minY && ogreVPos.y < maxY)
                {
                    pickIndex.push_back(pid);
                }
            }
        }
    }

    void PickPointTool::PickPointSetByCycle(const MagicDGP::Point3DSet* pPS, MagicMath::Vector2 centerPos, 
            double radius, std::vector<int>& pickIndex, bool ignoreBack)
    {
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        if (pPS->HasNormal() && ignoreBack)
        {
            int pointNum = pPS->GetPointNumber();
            for (int pid = 0; pid < pointNum; pid++)
            {
                MagicMath::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                if ((screenPos - centerPos).Length() < radius)
                {
                    MagicMath::Vector3 vNor = pPS->GetPoint(pid)->GetNormal();
                    Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                    ogreVNor = worldM * ogreVNor;
                    if (ogreVNor.z > 0)
                    {
                        pickIndex.push_back(pid);
                    }
                }
            }
        }
        else
        {
            int pointNum = pPS->GetPointNumber();
            for (int pid = 0; pid < pointNum; pid++)
            {
                MagicMath::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                MagicMath::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                if ((screenPos - centerPos).Length() < radius)
                {
                    pickIndex.push_back(pid);
                }
            }
        }
    }

    void PickPointTool::SetPickParameter(PickMode pm, bool ignoreBack, MagicDGP::LightMesh3D* pLightMesh, MagicDGP::Mesh3D* pMesh, MagicDGP::Point3DSet* pPS)
    {
        mPickMode = pm;
        mIgnoreBack = ignoreBack;
        mpLightMesh = pLightMesh;
        mpMesh = pMesh;
        mpPointSet = pPS;
    }

    void PickPointTool::MousePressed(const OIS::MouseEvent& arg)
    {
        mMousePos = MagicMath::Vector2(arg.state.X.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getWidth() - 1.0, 
                    1.0 - arg.state.Y.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getHeight());
        mPickPressed = true;
    }

    void PickPointTool::MouseMoved(const OIS::MouseEvent& arg)
    {
        if (arg.state.buttonDown(OIS::MB_Left) == true)
        {
            MagicMath::Vector2 curPos(arg.state.X.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getWidth() - 1.0, 
                    1.0 - arg.state.Y.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getHeight());
            UpdateMarkObject(mMousePos, curPos);
        }
    }

    void PickPointTool::MouseReleased(const OIS::MouseEvent& arg)
    {
        mPickMeshIndex.clear();
        mPickLightMeshIndex.clear();
        mPickPointsetIndex.clear();
        if (mPickPressed)
        {
            MagicMath::Vector2 curPos(arg.state.X.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getWidth() - 1.0, 
                    1.0 - arg.state.Y.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getHeight());
            ClearMarkObject();
            Pick(curPos);
            mPickPressed = false;
        }

    }

    void PickPointTool::GetPickMeshIndex(std::vector<int>& pickIndex)
    {
        pickIndex = mPickMeshIndex;
    }

    void PickPointTool::GetPickLightMeshIndex(std::vector<int>& pickIndex)
    {
        pickIndex = mPickLightMeshIndex;
    }

    void PickPointTool::GetPickPointsetIndex(std::vector<int>& pickIndex)
    {
        pickIndex = mPickPointsetIndex;
    }

    void PickPointTool::UpdateMarkObject(MagicMath::Vector2& pos0, MagicMath::Vector2& pos1)
    {
        if (mPickMode != PM_Point)
        {
            Ogre::ManualObject* pMObj = NULL;
            Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
            if (pSceneMgr->hasManualObject("PickObj"))
            {
                pMObj = pSceneMgr->getManualObject("PickObj");
                pMObj->clear();
            }
            else
            {
                pMObj = pSceneMgr->createManualObject("PickObj");
                pMObj->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
                pMObj->setUseIdentityProjection(true);
                pMObj->setUseIdentityView(true);
                if (pSceneMgr->hasSceneNode("PickMarkNode"))
                {
                    pSceneMgr->getSceneNode("PickMarkNode")->attachObject(pMObj);
                }
                else
                {
                    pSceneMgr->getRootSceneNode()->createChildSceneNode("PickMarkNode")->attachObject(pMObj);
                }
            }
            if (mPickMode == PM_Rectangle)
            {
                pMObj->begin("SimpleLine", Ogre::RenderOperation::OT_LINE_STRIP);
                pMObj->position(pos0[0], pos0[1], -1);
                pMObj->colour(0.615, 0.16, 0.196);
                pMObj->position(pos0[0], pos1[1], -1);
                pMObj->colour(0.615, 0.16, 0.196);
                pMObj->position(pos1[0], pos1[1], -1);
                pMObj->colour(0.615, 0.16, 0.196);
                pMObj->position(pos1[0], pos0[1], -1);
                pMObj->colour(0.615, 0.16, 0.196);
                pMObj->position(pos0[0], pos0[1], -1);
                pMObj->colour(0.615, 0.16, 0.196);
                pMObj->end();
            }
            else if (mPickMode == PM_Cycle)
            {
                pMObj->begin("SimpleLine", Ogre::RenderOperation::OT_LINE_STRIP);
                float twoPi = 8.0f * atan( 1.0f );
                double len = (pos0 - pos1).Length();
                for (int i = 0; i < 41; i++)
                {
                    double theta = twoPi * i / 40.f;
                    double x = pos0[0] + len * cos(theta);
                    double y = pos0[1] + len * sin(theta);
                    pMObj->position(x, y, -1);
                    pMObj->colour(0.615, 0.16, 0.196);
                }
                pMObj->end();
            }
        }
    }

    void PickPointTool::ClearMarkObject()
    {
        if (MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasManualObject("PickObj"))
        {
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->destroyManualObject("PickObj");
        }
    }

    void PickPointTool::Pick(MagicMath::Vector2& curPos)
    {
        if (mpMesh != NULL)
        {
            if (mPickMode == PM_Point)
            {
                mPickMeshIndex.clear();
                int pickIndex = PickMeshVertexByPoint(mpMesh, curPos, mIgnoreBack);
                if (pickIndex != -1)
                {
                    mPickMeshIndex.push_back(pickIndex);
                }
            }
            else if (mPickMode == PM_Rectangle)
            {
                PickMeshVertexByRectangle(mpMesh, mMousePos, curPos, mPickMeshIndex, mIgnoreBack);
            }
            else if (mPickMode == PM_Cycle)
            {
                PickMeshVertexByCycle(mpMesh, mMousePos, (curPos - mMousePos).Length(), mPickMeshIndex, mIgnoreBack);
            }
        }
        if (mpLightMesh != NULL)
        {
            if (mPickMode == PM_Point)
            {
                mPickLightMeshIndex.clear();
                int pickIndex = PickMeshVertexByPoint(mpLightMesh, curPos, mIgnoreBack);
                if (pickIndex != -1)
                {
                    mPickLightMeshIndex.push_back(pickIndex);
                }
            }
            else if (mPickMode == PM_Rectangle)
            {
                PickMeshVertexByRectangle(mpLightMesh, mMousePos, curPos, mPickLightMeshIndex, mIgnoreBack);
            }
            else if (mPickMode == PM_Cycle)
            {
                PickMeshVertexByCycle(mpLightMesh, mMousePos, (curPos - mMousePos).Length(), mPickLightMeshIndex, mIgnoreBack);
            }
        }
        if (mpPointSet != NULL)
        {
            if (mPickMode == PM_Point)
            {
                mPickPointsetIndex.clear();
                int pickIndex = PickPointSetByPoint(mpPointSet, curPos, mIgnoreBack);
                if (pickIndex != -1)
                {
                    mPickPointsetIndex.push_back(pickIndex);
                }
            }
            else if (mPickMode == PM_Rectangle)
            {
                PickPointSetByRectangle(mpPointSet, mMousePos, curPos, mPickPointsetIndex, mIgnoreBack);
            }
            else if (mPickMode == PM_Cycle)
            {
                PickPointSetByCycle(mpPointSet, mMousePos, (curPos - mMousePos).Length(), mPickPointsetIndex, mIgnoreBack);
            }
        }
    }
}
#include "PickPointTool.h"
#include "../Common/RenderSystem.h"

namespace MagicTool
{
    PickPointTool::PickPointTool() :
        mPickMode(PM_Point),
        mMousePos(0, 0),
        mpMesh(NULL),
        mpPointSet(NULL),
        mPickPressed(false)
    {
    }

    PickPointTool::~PickPointTool()
    {
    }

    int PickPointTool::PickMeshVertexByPoint(const MagicDGP::Mesh3D* pMesh, MagicDGP::Vector2 mousePos)
    {
        if (MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasSceneNode("ModelNode") == false)
        {
            return -1;
        }
        MagicDGP::Real pointSizeSquared = 0.01 * 0.01;
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        MagicDGP::Real minZ = 1.0e10;
        int pickIndex = -1;
        int vertNum = pMesh->GetVertexNumber();
        for (int vid = 0; vid < vertNum; vid++)
        {
            MagicDGP::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
            MagicDGP::Vector3 vNor = pMesh->GetVertex(vid)->GetNormal();
            Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
            Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
            ogreVPos = wvpM * ogreVPos;
            MagicDGP::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
            if ((screenPos - mousePos).LengthSquared() < pointSizeSquared)
            {
                ogreVNor = worldM * ogreVNor;
                if (ogreVNor.z > 0)
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

    void PickPointTool::PickMeshVertexByRectangle(const MagicDGP::Mesh3D* pMesh, MagicDGP::Vector2 pos0, 
            MagicDGP::Vector2 pos1, std::vector<int>& pickIndex)
    {
        pickIndex.clear();
        MagicDGP::Real minX = (pos0[0] < pos1[0]) ? pos0[0] : pos1[0];
		MagicDGP::Real maxX = (pos0[0] > pos1[0]) ? pos0[0] : pos1[0];
		MagicDGP::Real minY = (pos0[1] < pos1[1]) ? pos0[1] : pos1[1];
		MagicDGP::Real maxY = (pos0[1] > pos1[1]) ? pos0[1] : pos1[1];

        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;

        int vertNum = pMesh->GetVertexNumber();
        for (int vid = 0; vid < vertNum; vid++)
        {
            MagicDGP::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
            Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
            ogreVPos = wvpM * ogreVPos;
            if (ogreVPos.x > minX && ogreVPos.x < maxX && ogreVPos.y > minY && ogreVPos.y < maxY)
            {
                MagicDGP::Vector3 vNor = pMesh->GetVertex(vid)->GetNormal();
                Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                ogreVNor = worldM * ogreVNor;
                if (ogreVNor.z > 0)
                {
                    pickIndex.push_back(vid);
                }
            }
        }
    }

    void PickPointTool::PickMeshVertexByCycle(const MagicDGP::Mesh3D* pMesh, MagicDGP::Vector2 centerPos, 
            MagicDGP::Real radius, std::vector<int>& pickIndex)
    {
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        int vertNum = pMesh->GetVertexNumber();
        for (int vid = 0; vid < vertNum; vid++)
        {
            MagicDGP::Vector3 vPos = pMesh->GetVertex(vid)->GetPosition();
            Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
            ogreVPos = wvpM * ogreVPos;
            MagicDGP::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
            if ((screenPos - centerPos).Length() < radius)
            {
                MagicDGP::Vector3 vNor = pMesh->GetVertex(vid)->GetNormal();
                Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                ogreVNor = worldM * ogreVNor;
                if (ogreVNor.z > 0)
                {
                    pickIndex.push_back(vid);
                }
            }
        }
    }

    int PickPointTool::PickPointSetByPoint(const MagicDGP::Point3DSet* pPS, MagicDGP::Vector2 mousePos)
    {
        if (MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasSceneNode("ModelNode") == false)
        {
            return -1;
        }
        MagicDGP::Real pointSizeSquared = 0.01 * 0.01;
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        MagicDGP::Real minZ = 1.0e10;
        int pickIndex = -1;
        int pointNum = pPS->GetPointNumber();
        if (pPS->HasNormal())
        {
            for (int pid = 0; pid < pointNum; pid++)
            {
                MagicDGP::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                MagicDGP::Vector3 vNor = pPS->GetPoint(pid)->GetNormal();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                Ogre::Vector4 ogreVNor(vNor[0], vNor[1], vNor[2], 0);
                ogreVPos = wvpM * ogreVPos;
                MagicDGP::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
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
                MagicDGP::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                MagicDGP::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
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

    void PickPointTool::PickPointSetByRectangle(const MagicDGP::Point3DSet* pPS, MagicDGP::Vector2 pos0,
            MagicDGP::Vector2 pos1, std::vector<int>& pickIndex)
    {
        pickIndex.clear();
        MagicDGP::Real minX = (pos0[0] < pos1[0]) ? pos0[0] : pos1[0];
		MagicDGP::Real maxX = (pos0[0] > pos1[0]) ? pos0[0] : pos1[0];
		MagicDGP::Real minY = (pos0[1] < pos1[1]) ? pos0[1] : pos1[1];
		MagicDGP::Real maxY = (pos0[1] > pos1[1]) ? pos0[1] : pos1[1];

        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        if (pPS->HasNormal())
        {
            int pointNum = pPS->GetPointNumber();
            for (int pid = 0; pid < pointNum; pid++)
            {
                MagicDGP::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                if (ogreVPos.x > minX && ogreVPos.x < maxX && ogreVPos.y > minY && ogreVPos.y < maxY)
                {
                    MagicDGP::Vector3 vNor = pPS->GetPoint(pid)->GetNormal();
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
                MagicDGP::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                if (ogreVPos.x > minX && ogreVPos.x < maxX && ogreVPos.y > minY && ogreVPos.y < maxY)
                {
                    pickIndex.push_back(pid);
                }
            }
        }
    }

    void PickPointTool::PickPointSetByCycle(const MagicDGP::Point3DSet* pPS, MagicDGP::Vector2 centerPos, 
            MagicDGP::Real radius, std::vector<int>& pickIndex)
    {
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->_getFullTransform();
        Ogre::Matrix4 viewM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getViewMatrix();
        Ogre::Matrix4 projM  = MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->getProjectionMatrix();
        Ogre::Matrix4 wvpM   = projM * viewM * worldM;
        if (pPS->HasNormal())
        {
            int pointNum = pPS->GetPointNumber();
            for (int pid = 0; pid < pointNum; pid++)
            {
                MagicDGP::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                MagicDGP::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                if ((screenPos - centerPos).Length() < radius)
                {
                    MagicDGP::Vector3 vNor = pPS->GetPoint(pid)->GetNormal();
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
                MagicDGP::Vector3 vPos = pPS->GetPoint(pid)->GetPosition();
                Ogre::Vector3 ogreVPos(vPos[0], vPos[1], vPos[2]);
                ogreVPos = wvpM * ogreVPos;
                MagicDGP::Vector2 screenPos(ogreVPos.x, ogreVPos.y);
                if ((screenPos - centerPos).Length() < radius)
                {
                    pickIndex.push_back(pid);
                }
            }
        }
    }

    void PickPointTool::SetPickParameter(PickMode pm, MagicDGP::Mesh3D* pMesh, MagicDGP::Point3DSet* pPS)
    {
        mPickMode = pm;
        mpMesh = pMesh;
        mpPointSet = pPS;
    }

    void PickPointTool::MousePressed(const OIS::MouseEvent& arg)
    {
        mMousePos = MagicDGP::Vector2(arg.state.X.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getWidth() - 1.0, 
                    1.0 - arg.state.Y.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getHeight());
        mPickPressed = true;
    }

    void PickPointTool::MouseMoved(const OIS::MouseEvent& arg)
    {
        if (arg.state.buttonDown(OIS::MB_Left) == true)
        {
            MagicDGP::Vector2 curPos(arg.state.X.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getWidth() - 1.0, 
                    1.0 - arg.state.Y.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getHeight());
            UpdateMarkObject(mMousePos, curPos);
        }
    }

    void PickPointTool::MouseReleased(const OIS::MouseEvent& arg)
    {
        mPickMeshIndex.clear();
        mPickPointsetIndex.clear();
        if (mPickPressed)
        {
            MagicDGP::Vector2 curPos(arg.state.X.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getWidth() - 1.0, 
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

    void PickPointTool::GetPickPointsetIndex(std::vector<int>& pickIndex)
    {
        pickIndex = mPickPointsetIndex;
    }

    void PickPointTool::UpdateMarkObject(MagicDGP::Vector2& pos0, MagicDGP::Vector2& pos1)
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
                MagicDGP::Real len = (pos0 - pos1).Length();
                for (int i = 0; i < 41; i++)
                {
                    MagicDGP::Real theta = twoPi * i / 40.f;
                    MagicDGP::Real x = pos0[0] + len * cos(theta);
                    MagicDGP::Real y = pos0[1] + len * sin(theta);
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

    void PickPointTool::Pick(MagicDGP::Vector2& curPos)
    {
        if (mpMesh != NULL)
        {
            if (mPickMode == PM_Point)
            {
                mPickMeshIndex.clear();
                int pickIndex = PickMeshVertexByPoint(mpMesh, curPos);
                if (pickIndex != -1)
                {
                    mPickMeshIndex.push_back(pickIndex);
                }
            }
            else if (mPickMode == PM_Rectangle)
            {
                PickMeshVertexByRectangle(mpMesh, mMousePos, curPos, mPickMeshIndex);
            }
            else if (mPickMode == PM_Cycle)
            {
                PickMeshVertexByCycle(mpMesh, mMousePos, (curPos - mMousePos).Length(), mPickMeshIndex);
            }
        }
        if (mpPointSet != NULL)
        {
            if (mPickMode == PM_Point)
            {
                mPickPointsetIndex.clear();
                int pickIndex = PickPointSetByPoint(mpPointSet, curPos);
                if (pickIndex != -1)
                {
                    mPickPointsetIndex.push_back(pickIndex);
                }
            }
            else if (mPickMode == PM_Rectangle)
            {
                PickPointSetByRectangle(mpPointSet, mMousePos, curPos, mPickPointsetIndex);
            }
            else if (mPickMode == PM_Cycle)
            {
                PickPointSetByCycle(mpPointSet, mMousePos, (curPos - mMousePos).Length(), mPickPointsetIndex);
            }
        }
    }
}
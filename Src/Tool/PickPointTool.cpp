#include "PickPointTool.h"
#include "../Common/RenderSystem.h"

namespace MagicTool
{
    PickPointTool::PickPointTool() :
        mPickMode(PM_Point),
        mMousePos(0, 0),
        mpMesh(NULL),
        mpPointSet(NULL)
    {
    }

    PickPointTool::~PickPointTool()
    {
    }

    int PickPointTool::PickMeshVertexByPoint(const MagicDGP::Mesh3D* pMesh, MagicDGP::Vector2 mousePos)
    {
        MagicDGP::Real pointSizeSquared = 0.01 * 0.01;
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->_getFullTransform();
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

    }

    void PickPointTool::PickMeshVertexByCycle(const MagicDGP::Mesh3D* pMesh, MagicDGP::Vector2 centerPos, 
            MagicDGP::Real radius, std::vector<int>& pickIndex)
    {

    }

    int PickPointTool::PickPointSetByPoint(const MagicDGP::Point3DSet* pPS, MagicDGP::Vector2 mousePos)
    {
        MagicDGP::Real pointSizeSquared = 0.01 * 0.01;
        Ogre::Matrix4 worldM = MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->_getFullTransform();
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

    }

    void PickPointTool::PickPointSetByCycle(const MagicDGP::Point3DSet* pPS, MagicDGP::Vector2 centerPos, 
            MagicDGP::Real radius, std::vector<int>& pickIndex)
    {

    }

    void PickPointTool::SetPickParameter(PickMode pm, MagicDGP::Mesh3D* pMesh, MagicDGP::Point3DSet* pPS)
    {

    }

    void PickPointTool::MousePressed(const OIS::MouseEvent& arg)
    {
        mMousePos = MagicDGP::Vector2(arg.state.X.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getWidth() - 1.0, 
                    1.0 - arg.state.Y.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getHeight());
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
        MagicDGP::Vector2 curPos(arg.state.X.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getWidth() - 1.0, 
                    1.0 - arg.state.Y.abs * 2.0 / MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getHeight());
        ClearMarkObject();
        Pick(curPos);
    }

    void PickPointTool::GetPickMeshIndex(std::vector<int>& pickIndex)
    {

    }

    void PickPointTool::GetPickPointsetIndex(std::vector<int>& pickIndex)
    {

    }

    void PickPointTool::UpdateMarkObject(MagicDGP::Vector2& pos0, MagicDGP::Vector2& pos1)
    {

    }

    void PickPointTool::ClearMarkObject()
    {

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
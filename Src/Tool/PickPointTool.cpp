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

    }

    void PickPointTool::MouseMoved(const OIS::MouseEvent& arg)
    {

    }

    void PickPointTool::MouseReleased(const OIS::MouseEvent& arg)
    {

    }

    void PickPointTool::GetPickMeshIndex(std::vector<int>& pickIndex)
    {

    }

    void PickPointTool::GetPickPointsetIndex(std::vector<int>& pickIndex)
    {

    }

    void PickPointTool::UpdateMarkObject()
    {

    }

    void PickPointTool::ClearMarkObject()
    {

    }
}
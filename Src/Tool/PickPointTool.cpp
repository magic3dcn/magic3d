#include "PickPointTool.h"
#include "../Common/RenderSystem.h"

namespace MagicTool
{
    PickPointTool::PickPointTool()
    {
    }

    PickPointTool::~PickPointTool()
    {
    }

    int PickPointTool::PickMeshVertex(const MagicDGP::Mesh3D* pMesh, MagicDGP::Vector2 mousePos)
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
}
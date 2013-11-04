#include "Curvature.h"

namespace MagicDGP
{
    Curvature::Curvature()
    {
    }

    Curvature::~Curvature()
    {
    }

    void Curvature::CalGaussianCurvature(const Mesh3D* pMesh, std::vector<Real>& curvList)
    {
        int vertNum = pMesh->GetVertexNumber();
        curvList.clear();
        curvList.resize(vertNum);
        Real twoPI = 2 * 3.14159265;
        for (int vid = 0; vid < vertNum; vid++)
        {
            const Vertex3D* pVert = pMesh->GetVertex(vid);
            Vector3 pos = pVert->GetPosition();
            const Edge3D* pEdge = pVert->GetEdge();
            Real angleSum = 0;
            do
            {
                const Face3D* pFace = pEdge->GetFace();
                if (pFace != NULL)
                {
                    Vector3 pos1 = pEdge->GetVertex()->GetPosition();
                    Vector3 pos2 = pEdge->GetNext()->GetVertex()->GetPosition();
                    Vector3 dir1 = pos1 - pos;
                    dir1.Normalise();
                    Vector3 dir2 = pos2 - pos;
                    dir2.Normalise();
                    Real cosAngle = dir1 * dir2;
                    cosAngle = cosAngle > 1 ? 1 : (cosAngle < -1 ? -1 : cosAngle);
                    angleSum += acos(cosAngle);
                }
                pEdge = pEdge->GetPair()->GetNext();
            } while (pEdge != NULL && pEdge != pVert->GetEdge());
            curvList.at(vid) = twoPI - angleSum;
        }
    }
}
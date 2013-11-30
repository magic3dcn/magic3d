#pragma once
#include "PointCloud3D.h"
#include "Mesh3D.h"

namespace MagicDGP
{
    class Filter
    {
    public:
        Filter();
        ~Filter();

    public:
        static Mesh3D* RemoveSmallMeshPatch(Mesh3D* pMesh);
        static void SimpleMeshSmooth(Mesh3D* pMesh);
        static Point3DSet* RemovePointSetOutlier(Point3DSet* pPS, Real proportion);
        static Point3DSet* PointSetSampling(Point3DSet* pPS, int sampleNum);
        static int MeshVertexSampling(const Mesh3D* pMesh, int sampleNum, std::vector<int>& sampleIndex);
        static bool SimplifyMesh(Mesh3D* pMesh, int targetNum);

    private:
        static int ChooseCollapseEdge(Mesh3D* pMesh, std::vector<std::vector<Real> >& quadricMatrix);
        static void CalQuadricMatrix(Mesh3D* pMesh, std::vector<std::vector<Real> >& quadricMatList);
        static void CollapseEdge(Mesh3D* pMesh, int edgeId, std::vector<std::vector<Real> >& quadricMatList);
    };
}
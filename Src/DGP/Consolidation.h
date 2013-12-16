#pragma once
#include "PointCloud3D.h"
#include "Mesh3D.h"

namespace MagicDGP
{
    class Consolidation
    {
    public:
        Consolidation();
        ~Consolidation();

        static void CalPointSetNormal(Point3DSet* pPointSet);
        static bool RedressPointSetNormal(Point3DSet* pPointSet);

        static Mesh3D* RemoveSmallMeshPatch(Mesh3D* pMesh, Real proportion);
        static LightMesh3D* RemoveSmallMeshPatch(LightMesh3D* pMesh, Real proportion);
        static Point3DSet* RemovePointSetOutlier(Point3DSet* pPS, Real proportion);

        static void SimpleMeshSmooth(Mesh3D* pMesh);
        static void SimpleMeshSmooth(LightMesh3D* pMesh);
        static void MeanCurvatureFlowFairing(Mesh3D* pMesh); 
        static void SimplePointsetSmooth(Point3DSet* pPS, std::vector<std::vector<int> >& RiemannianGraph, bool needConstructGraph);

    };

}
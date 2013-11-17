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

    };
}
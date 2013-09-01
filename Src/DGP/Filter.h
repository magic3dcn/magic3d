#pragma once
#include "PointCloud3D.h"

namespace MagicDGP
{
    class Filter
    {
    public:
        Filter();
        ~Filter();

    public:
        static void FilterDepthScanPointSet(Point3DSet* pPCSet);

    private:
        static void FindNearestNeighbors(Point3DSet* pPCSet, int nn, int** pIndex, float** pDist);

    };
}
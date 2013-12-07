#pragma once
#include "PointCloud3D.h"

namespace MagicDGP
{
    class Consolidation
    {
    public:
        Consolidation();
        ~Consolidation();

        static void CalPointSetNormal(Point3DSet* pPointSet);
        static bool RedressPointSetNormal(Point3DSet* pPointSet);

    private:

    };

}
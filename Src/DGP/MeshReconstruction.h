#pragma once
#include "PointCloud3D.h"
#include "Mesh3D.h"

namespace MagicDGP
{
    class MeshReconstruction
    {
    public:
        MeshReconstruction();
        ~MeshReconstruction();

        static Mesh3D* ScreenPoissonReconstruction(const Point3DSet* pPC);

    private:

    };
}
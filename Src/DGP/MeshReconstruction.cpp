#include "StdAfx.h"
#include "MeshReconstruction.h"
#include "../Dependence/PoissonReconstruction.h"

namespace MagicDGP
{
    MeshReconstruction::MeshReconstruction()
    {
    }

    MeshReconstruction::~MeshReconstruction()
    {
    }

    Mesh3D* MeshReconstruction::ScreenPoissonReconstruction(const Point3DSet* pPC)
    {
        char* argv1[] = {"--in", "pc.psr", "--out", "pc.ply", "--depth", "10", "--density"};
        MagicDependence::PoissonReconstruction::PoissonRecon(7, argv1, NULL);
        //char* argv2[] = {"--in", "pc.ply", "--out", "pct.ply", "--trim", "7", "--aRatio", "0"};
        //MagicDependence::PoissonReconstruction::SurfaceTrimmer(8, argv2, NULL);
        return NULL;
    }
}
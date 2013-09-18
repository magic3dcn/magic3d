#include "PrimitiveDetection.h"

namespace MagicDGP
{
    PrimitiveDetection::PrimitiveDetection()
    {
    }

    PrimitiveDetection::~PrimitiveDetection()
    {
    }

    void PrimitiveDetection::Primitive2DDetection(const Mesh3D* pMesh, std::vector<int>& res)
    {
        MagicDependence::RansacPrimitiveShapes::MeshPrimitiveDetection(pMesh, res);
    }
}
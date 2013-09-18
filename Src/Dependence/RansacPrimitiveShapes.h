#pragma once
#include <vector>
#include "../DGP/Mesh3D.h"

namespace MagicDependence
{
    class RansacPrimitiveShapes
    {
    public:
        RansacPrimitiveShapes();
        ~RansacPrimitiveShapes();

        static void MeshPrimitiveDetection(const MagicDGP::Mesh3D* pMesh, std::vector<int>& res);
    
    private:

    };

}

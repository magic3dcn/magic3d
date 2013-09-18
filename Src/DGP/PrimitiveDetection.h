#pragma once 
#include "../Dependence/RansacPrimitiveShapes.h"

namespace MagicDGP
{
    class PrimitiveDetection
    {
    public:
        PrimitiveDetection();
        ~PrimitiveDetection();

        static void Primitive2DDetection(const Mesh3D* pMesh, std::vector<int>& res);

    private:

    };

}
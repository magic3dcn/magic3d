#pragma once
#include "../DGP/Mesh3D.h"
#include "../DGP/Vector2.h"

namespace MagicTool
{
    class PickPointTool
    {
    public:
        PickPointTool();
        ~PickPointTool();

        static int PickMeshVertex(const MagicDGP::Mesh3D* pMesh, MagicDGP::Vector2 mousePos);
    };
}
#pragma once
#include "PointCloud3D.h"
#include "Mesh3D.h"

namespace MagicDGP
{
    class Parser
    {
    public:
        Parser();
        ~Parser();

        Point3DSet* ParsePointSet(std::string fileName);
        Mesh3D*     ParseMesh3D(std::string fileName);

    private:
        Point3DSet* ParsePointSetByOBJ(std::string fileName);
        Mesh3D*     ParseMesh3DByOBJ(std::string fileName);
    };
}
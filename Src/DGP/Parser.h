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

        static Point3DSet*  ParsePointSet(std::string fileName);
        static Mesh3D*      ParseMesh3D(std::string fileName);
        static LightMesh3D* ParseLightMesh3D(std::string fileName);

        static void ExportPointSet(std::string fileName, const Point3DSet* pPC);
        static void ExportMesh3D(std::string fileName, const Mesh3D* pMesh);
        static void ExportLightMesh3D(std::string fileName, const LightMesh3D* pMesh);

    private:
        static Point3DSet* ParsePointSetByOBJ(std::string fileName);
        static Point3DSet* ParsePointSetBySTL(std::string fileName);
        static Point3DSet* ParsePointSetByOFF(std::string fileName);

        static Mesh3D* ParseMesh3DByOBJ(std::string fileName);
        static Mesh3D* ParseMesh3dBySTL(std::string fileName);
        static Mesh3D* ParseMesh3dByOFF(std::string fileName);

        static LightMesh3D* ParseLightMesh3DByOBJ(std::string fileName);
        static LightMesh3D* ParseLightMesh3dBySTL(std::string fileName);
        static LightMesh3D* ParseLightMesh3dByOFF(std::string fileName);

        static void ExportPointSetByOBJ(std::string fileName, const Point3DSet* pPC);
        static void ExportPointSetByPLY(std::string fileName, const Point3DSet* pPC);
        static void ExportPointSetByOFF(std::string fileName, const Point3DSet* pPC);

        static void ExportMesh3DByOBJ(std::string fileName, const Mesh3D* pMesh);
        static void ExportMesh3DBySTL(std::string fileName, const Mesh3D* pMesh);
        static void ExportMesh3DByOFF(std::string fileName, const Mesh3D* pMesh);

        static void ExportLightMesh3DByOBJ(std::string fileName, const LightMesh3D* pMesh);
        static void ExportLightMesh3DBySTL(std::string fileName, const LightMesh3D* pMesh);
        static void ExportLightMesh3DByOFF(std::string fileName, const LightMesh3D* pMesh);
    };
}
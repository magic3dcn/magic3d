#pragma once
#include "PointCloud3D.h"
#include "Mesh3D.h"
#include "HomoMatrix4.h"

namespace MagicDGP
{
    class Sampling
    {
    public:
        Sampling();
        ~Sampling();
        //PointSet need CalculateBBox and CalculateDensity
        static Point3DSet* PointSetWLOPSampling(const Point3DSet* pPS, int sampleNum);
        static Point3DSet* PointSetUniformSampling(Point3DSet* pPS, int sampleNum);
        static int MeshVertexUniformSampling(const Mesh3D* pMesh, int sampleNum, std::vector<int>& sampleIndex);
        static bool SimplifyMesh(Mesh3D* pMesh, int targetNum);

    private:
        static int ChooseCollapseEdge(Mesh3D* pMesh, std::vector<HomoMatrix4>& quadricMatrix);
        static void CalQuadricMatrix(Mesh3D* pMesh, std::vector<HomoMatrix4>& quadricMatList);
        static void CollapseEdge(Mesh3D* pMesh, int edgeId, std::vector<HomoMatrix4>& quadricMatList);

        static void InitialSampling(const Point3DSet* pPS, int sampleNum, std::vector<Vector3>& samplePosList);
        static void WLOPIteration(const Point3DSet* pPS, std::vector<Vector3> & samplePosList);
        static void LocalPCANormalEstimate(const std::vector<Vector3>& samplePosList, std::vector<Vector3>& norList);
        static void NormalConsistent(const Point3DSet* pPS, std::vector<Vector3>& samplePosList, std::vector<Vector3>& norList);
        static void NormalSmooth(std::vector<Vector3>& samplePosList, std::vector<Vector3>& norList);
    };


}
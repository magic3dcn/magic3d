#pragma once
#include "Mesh3D.h"

namespace MagicDGP
{
    class Curvature
    {
    public:
        Curvature();
        ~Curvature();

        static void CalGaussianCurvature(const Mesh3D* pMesh, std::vector<double>& curvList);
        static void CalMeanCurvature(const Mesh3D* pMesh, std::vector<double>& curvList);
    };
}
#pragma once
#include "PointCloud3D.h"
#include <vector>

namespace MagicDGP
{
    class Registration
    {
    public:
        Registration();
        ~Registration();

    public:
        static void ICPRegistrate(const Point3DSet* pRef, Point3DSet* pPC);
        static void ICPRegistrate(const Point3DSet* pOrigin, Point3DSet* pTarget, float* pTransInit, float* pTransRes); 

    private:
        static void ICPSamplePoint(const Point3DSet* pPC, std::vector<int>& sampleIndex);
        static void ICPFindCorrespondance(const Point3DSet* pRef, const Point3DSet* pPC, 
            std::vector<int>& sampleIndex,  std::vector<int>& correspondIndex);
        static void ICPEnergyMinimization(const Point3DSet* pRef, Point3DSet* pPC, 
            std::vector<int>& sampleIndex, std::vector<int>& correspondIndex);
    };


}
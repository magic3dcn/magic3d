#pragma once
#include "PointCloud3D.h"
#include <vector>
#include "HomoMatrix4.h"

namespace MagicDGP
{
    class Registration
    {
    public:
        Registration();
        ~Registration();

    public:
        static void ICPRegistrate(const Point3DSet* pRef, Point3DSet* pOrigin);
        static void ICPRegistrate(const Point3DSet* pRef, Point3DSet* pOrigin, const HomoMatrix4* pTransInit, HomoMatrix4* pTransRes); 

    private:
        static void ICPSamplePoint(const Point3DSet* pPC, std::vector<int>& sampleIndex);
        static void ICPFindCorrespondance(const Point3DSet* pRef, const Point3DSet* pOrigin, 
            std::vector<int>& sampleIndex,  std::vector<int>& correspondIndex);
        static void ICPFindCorrespondance(const Point3DSet* pRef, const Point3DSet* pOrigin, const HomoMatrix4* pTransInit,
            std::vector<int>& sampleIndex,  std::vector<int>& correspondIndex);
        static void ICPEnergyMinimization(const Point3DSet* pRef, Point3DSet* pOrigin, 
            std::vector<int>& sampleIndex, std::vector<int>& correspondIndex);
        static void ICPEnergyMinimization(const Point3DSet* pRef, const Point3DSet* pOrigin, const HomoMatrix4* pTransInit,
            std::vector<int>& sampleIndex, std::vector<int>& correspondIndex, HomoMatrix4* pTransDelta);
    };


}
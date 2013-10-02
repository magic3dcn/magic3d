#pragma once
#include "PointCloud3D.h"
#include <vector>
#include "HomoMatrix4.h"
#include "flann/flann.h"

namespace MagicDGP
{
    class Registration
    {
    public:
        Registration();
        ~Registration();

    public:
        void ICPRegistrate(const Point3DSet* pRef, Point3DSet* pOrigin, const HomoMatrix4* pTransInit, HomoMatrix4* pTransRes); 

    private:
        void ICPInitRefData(const Point3DSet* pRef);
        void ICPSamplePoint(const Point3DSet* pPC, std::vector<int>& sampleIndex);
        void ICPFindCorrespondance(const Point3DSet* pRef, const Point3DSet* pOrigin, const HomoMatrix4* pTransInit,
            std::vector<int>& sampleIndex,  std::vector<int>& correspondIndex);
        void ICPEnergyMinimization(const Point3DSet* pRef, const Point3DSet* pOrigin, const HomoMatrix4* pTransInit,
            std::vector<int>& sampleIndex, std::vector<int>& correspondIndex, HomoMatrix4* pTransDelta);

    private:
        flann_index_t mFlannIndex;
        float* mDataSet;
        FLANNParameters mSearchPara;
    };


}
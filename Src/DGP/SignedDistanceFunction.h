#pragma once
#include "PointCloud3D.h"
#include "../Math/HomoMatrix4.h"
#include <vector>
#include <set>

namespace MagicDGP
{
    class SignedDistanceFunction
    {
    public:
        SignedDistanceFunction(int resX, int resY, int resZ, float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
        ~SignedDistanceFunction();

        void UpdateSDF(const Point3DSet* pPC, const MagicMath::HomoMatrix4* pTransform);
        void UpdateFineSDF(const Point3DSet* pPC, const MagicMath::HomoMatrix4* pTransform);
        Point3DSet* ExtractPointCloud();
        Point3DSet* ExtractFinePointCloud();
        void ResetSDF();

    private:
        std::vector<float> mSDF;
        std::vector<float> mWeight;
        std::set<int>      mPCIndex;
        int mResolutionX;
        int mResolutionY;
        int mResolutionZ;
        float mMinX, mMinY, mMinZ;
        float mMaxX, mMaxY, mMaxZ;
    };


}
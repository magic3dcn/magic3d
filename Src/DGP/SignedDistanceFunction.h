#pragma once
#include "PointCloud3D.h"
#include <vector>

namespace MagicDGP
{
    class SignedDistanceFunction
    {
    public:
        SignedDistanceFunction(int resX, int resY, int resZ, float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
        ~SignedDistanceFunction();

        void UpdateSDF(const Point3DSet* pPC, float* pTransform);
        Point3DSet* PointCloudPrediction(float* pTransform);
        Point3DSet* ExtractPointCloud();

    private:
        std::vector<float> mSDF;
        std::vector<float> mWeight;
        int mResolutionX;
        int mResolutionY;
        int mResolutionZ;
        float mMinX, mMinY, mMinZ;
        float mMaxX, mMaxY, mMaxZ;
    };


}
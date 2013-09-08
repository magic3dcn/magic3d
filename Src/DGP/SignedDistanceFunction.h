#pragma once
#include "PointCloud3D.h"
#include <vector>
#include <set>

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
        Vector3 TransformPosition(const Vector3& pos, float* pTrans);

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
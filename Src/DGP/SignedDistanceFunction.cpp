#include "StdAfx.h"
#include "SignedDistanceFunction.h"

namespace MagicDGP
{
    SignedDistanceFunction::SignedDistanceFunction(int resX, int resY, int resZ, float minX, float maxX, float minY, float maxY, float minZ, float maxZ) :
        mSDF(resX * resY * resZ, 0.f),
        mWeight(resX * resY * resZ, 0.f),
        mResolutionX(resX),
        mResolutionY(resY),
        mResolutionZ(resZ),
        mMinX(minX),
        mMinY(minY),
        mMinZ(minZ),
        mMaxX(maxX),
        mMaxY(maxY),
        mMaxZ(maxZ)
    {

    }

    SignedDistanceFunction::~SignedDistanceFunction()
    {
    }

    void SignedDistanceFunction::UpdateSDF(const Point3DSet* pPC, float* pTransform)
    {

    }

    Point3DSet* SignedDistanceFunction::PointCloudPrediction(float* pTransform)
    {
        return NULL;
    }

    Point3DSet* SignedDistanceFunction::ExtractPointCloud()
    {
        return NULL;
    }
}
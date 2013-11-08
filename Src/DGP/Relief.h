#pragma once
#include "PointCloud3D.h"
#include "Mesh3D.h"
#include <vector>

namespace MagicDGP
{
    class ReliefGeneration
    {
    public:
        ReliefGeneration(int resX, int resY, Real minX, Real maxX, Real minY, Real maxY);
        Mesh3D* PlaneReliefFromPointCloud(const Point3DSet* pPC);
        Mesh3D* CylinderReliefFromPointCloud(const Point3DSet* pPC);
        ~ReliefGeneration();

    private:
        void InitHeightField(const Point3DSet* pPC);
        void CompressHeightField();
    private:
        std::vector<Real> mHeightField;
        int mResolutionX, mResolutionY;
        Real mMinX, mMaxX, mMinY, mMaxY;
    };

}
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
        Mesh3D* GenerationFromPointClout(Point3DSet* pPC);
        ~ReliefGeneration();

    private:
        std::vector<Real> mHeightField;
        int mResolutionX, mResolutionY;
        Real mMinX, mMaxX, mMinY, mMaxY;
    };

}
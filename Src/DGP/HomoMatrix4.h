#pragma once
#include "DGPDefines.h"
#include "Vector3.h"

namespace MagicDGP
{
    class HomoMatrix4
    {
    public:
        HomoMatrix4();
        HomoMatrix4(const HomoMatrix4& mat);
        HomoMatrix4(const Real* pValues);
        ~HomoMatrix4();

        Vector3 TransformPoint(const Vector3& vec) const;
        Vector3 RotateVector(const Vector3& vec) const;
        HomoMatrix4 operator * (const HomoMatrix4& mat) const;
        HomoMatrix4 operator *=(const HomoMatrix4& mat);
        Real GetValue(int row, int col) const;
        void SetValue(int row, int col, Real v);
        void Unit();

    private:
        Real mValues[16];
    };

}
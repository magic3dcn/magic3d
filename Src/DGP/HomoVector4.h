#pragma once
#include "DGPDefines.h"

namespace MagicDGP
{
    class HomoVector4
    {
    public:
        HomoVector4();
        HomoVector4(Real x, Real y, Real z, Real h);

        Real Distance(const HomoVector4 vec4) const;
        Real DistanceSquare(const HomoVector4 vec4) const;
        void NormalisePoint();

        Real NormaliseVector();
        HomoVector4 CrossProduct(const HomoVector4& vec4) const;

        Real     operator [] (int index) const;
        Real&    operator [] (int index);
        bool     operator == (const HomoVector4& vec4) const;
        HomoVector4  operator + (const HomoVector4& vec4) const;
        HomoVector4  operator - (const HomoVector4& vec4) const;
        HomoVector4  operator * (Real s) const;
        Real     operator * (const HomoVector4& vec4) const;
        HomoVector4  operator / (Real s) const;
        HomoVector4  operator - (void) const;
        HomoVector4& operator += (const HomoVector4& vec4);
        HomoVector4& operator -= (const HomoVector4& vec4);
        HomoVector4& operator *= (Real s);
        HomoVector4& operator /= (Real s);

        ~HomoVector4();

    private:
        Real mX, mY, mZ, mH;
    };
}
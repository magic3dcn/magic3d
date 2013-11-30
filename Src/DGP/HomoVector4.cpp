#include "HomoVector4.h"
#include <math.h>

namespace MagicDGP
{
    HomoVector4::HomoVector4()
    {
    }

    HomoVector4::HomoVector4(Real x, Real y, Real z, Real h) :
        mX(x),
        mY(y),
        mZ(z),
        mH(h)
    {
    }

    Real HomoVector4::Distance(const HomoVector4 vec4) const
    {
        return (*this - vec4).Length();
    }

    Real HomoVector4::DistanceSquare(const HomoVector4 vec4) const
    {
        return (*this - vec4).LengthSquare();
    }

    bool HomoVector4::NormalisePoint()
    {
        if (fabs(mH) > Epsilon)
        {
            mX /= mH;
            mY /= mH;
            mZ /= mH;
            mH = 1.0;

            return true;
        }
        else
        {
            return false;
        }
    }

    Real HomoVector4::Length() const
    {
        return sqrt(mX * mX + mY * mY + mZ * mZ);
    }

    Real HomoVector4::LengthSquare() const
    {
        return (mX * mX + mY * mY + mZ * mZ);
    }

    Real HomoVector4::NormaliseVector()
    {
        Real length = sqrt(mX * mX + mY * mY + mZ * mZ);
        if (length > Epsilon)
        {
            mX /= length;
            mY /= length;
            mZ /= length;
        }
        return length;
    }

    HomoVector4 HomoVector4::CrossProduct(const HomoVector4& vec4) const
    {
        return HomoVector4(mY * vec4[2] - mZ * vec4[1], mZ * vec4[0] - mX * vec4[2], mX * vec4[1] - mY * vec4[0], 0);
    }

    Real HomoVector4::operator [] (int index) const
    {
        MAGICASSERT((index < 3 && index > -1), "HomoVector4 index out of range!");
        return *(&mX + index);
    }

    Real& HomoVector4::operator [] (int index)
    {
        MAGICASSERT((index < 3 && index > -1), "HomoVector4 index out of range!");
        return *(&mX + index);
    }

    bool HomoVector4::operator == (const HomoVector4& vec4) const
    {
        if (fabs(mH) < Epsilon && fabs(vec4[3]) < Epsilon)
        {
            return ( (vec4 - *this).Length() < Epsilon );
        }
        else if (fabs(mH) > Epsilon && fabs(vec4[3]) > Epsilon)
        {
            HomoVector4 normPoint(mX / mH, mY / mH, mZ / mH, 1.0);
            HomoVector4 normVec4(vec4[0] / vec4[3], vec4[1] / vec4[3], vec4[2] / vec4[3], 1.0);
            return ( (normPoint - normVec4).Length() < Epsilon );
        }
        else
        {
            return false;
        }
    }

    HomoVector4  HomoVector4::operator + (const HomoVector4& vec4) const
    {
        return HomoVector4(mX + vec4[0], mY + vec4[1], mZ + vec4[2], mH + vec4[3]);
    }

    HomoVector4  HomoVector4::operator - (const HomoVector4& vec4) const
    {
        return HomoVector4(mX - vec4[0], mY - vec4[1], mZ - vec4[2], mH - vec4[3]);
    }

    HomoVector4  HomoVector4::operator * (Real s) const
    {
        return HomoVector4(mX * s, mY * s, mZ * s, mH * s);
    }

    Real HomoVector4::operator * (const HomoVector4& vec4) const
    {
        return (mX * vec4[0] + mY * vec4[1] + mZ * vec4[2] + mH * vec4[3]);
    }

    HomoVector4  HomoVector4::operator / (Real s) const
    {
        MAGICASSERT((fabs(s) > Epsilon), "Devidor is zero!");
        return HomoVector4(mX / s, mY / s, mZ / s, mH / s);
    }

    HomoVector4  HomoVector4::operator - (void) const
    {
        return HomoVector4(-mX, -mY, -mZ, -mH);
    }

    HomoVector4& HomoVector4::operator += (const HomoVector4& vec4)
    {
        mX += vec4[0];
        mY += vec4[1];
        mZ += vec4[2];
        mH += vec4[3];
        return *this;
    }

    HomoVector4& HomoVector4::operator -= (const HomoVector4& vec4)
    {
        mX -= vec4[0];
        mY -= vec4[1];
        mZ -= vec4[2];
        mH -= vec4[3];
        return *this;
    }

    HomoVector4& HomoVector4::operator *= (Real s)
    {
        mX *= s;
        mY *= s;
        mZ *= s;
        mH *= s;
        return *this;
    }

    HomoVector4& HomoVector4::operator /= (Real s)
    {
        MAGICASSERT((fabs(s) > Epsilon), "Devidor is zero!");
        mX /= s;
        mY /= s;
        mZ /= s;
        mH /= s;
        return *this;
    }

    HomoVector4::~HomoVector4()
    {
    }


}
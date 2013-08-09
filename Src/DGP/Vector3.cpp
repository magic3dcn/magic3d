#include "StdAfx.h"
#include "Vector3.h"
#include <math.h>

namespace MagicDGP
{
    Vector3::Vector3() : mX(0), mY(0), mZ(0)
    {
    }

    Vector3::Vector3(Real x, Real y, Real z) : mX(x), mY(y), mZ(z)
    {
    }

    Real Vector3::Length() const
    {
        return sqrt(mX * mX + mY * mY + mZ * mZ);
    }

    Real Vector3::LengthSquared() const
    {
        return mX * mX + mY * mY + mZ * mZ;
    }

    Real Vector3::Distance(const Vector3& vec3) const
    {
        return (*this - vec3).Length();
    }

    Real Vector3::DistanceSquared(const Vector3& vec3) const
    {
        return (*this - vec3).LengthSquared();
    }

    Real Vector3::Normalise()
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

    Vector3 Vector3::CrossProduct(const Vector3& vec3) const
    {
        return Vector3(mY * vec3[2] - mZ * vec3[1], mZ * vec3[0] - mX * vec3[2], mX * vec3[1] - mY * vec3[0]);
    }

    Real Vector3::operator [] (int index) const
    {
        MAGICASSERT((index < 3 && index > -1), "Vector3 index out of range!");
        return *(&mX + index);
    }

    Real& Vector3::operator [] (int index)
    {
        MAGICASSERT((index < 3 && index > -1), "Vector3 index out of range!");
        return *(&mX + index);
    }
    bool Vector3::operator == (const Vector3& vec3) const
    {
        return (mX == vec3[0] && mY == vec3[1] && mZ == vec3[2]);
    }

    Vector3 Vector3::operator + (const Vector3& vec3) const
    {
        return Vector3(mX + vec3[0], mY + vec3[1], mZ + vec3[2]);
    }

    Vector3 Vector3::operator - (const Vector3& vec3) const
    {
        return Vector3(mX - vec3[0], mY - vec3[1], mZ - vec3[2]);
    }

    Vector3 Vector3::operator * (Real s) const
    {
        return Vector3(mX * s, mY * s, mZ * s);
    }

    Real Vector3::operator * (const Vector3& vec3) const
    {
        return (mX * vec3[0] + mY * vec3[1] + mZ * vec3[2]);
    }

    Vector3 Vector3::operator / (Real s) const
    {
        MAGICASSERT((fabs(s) > Epsilon), "Devidor is zero!");
        return Vector3(mX / s, mY / s, mZ / s);
    }

    Vector3 Vector3::operator - (void) const
    {
        return Vector3(-mX, -mY, -mZ);
    }

    Vector3& Vector3::operator += (const Vector3& vec3)
    {
        mX += vec3[0];
        mY += vec3[1];
        mZ += vec3[2];
        return *this;
    }

    Vector3& Vector3::operator -= (const Vector3& vec3)
    {
        mX -= vec3[0];
        mY -= vec3[1];
        mZ -= vec3[2];
        return *this;
    }

    Vector3& Vector3::operator *= (Real s)
    {
        mX *= s;
        mY *= s;
        mZ *= s;
        return *this;
    }

    Vector3& Vector3::operator /= (Real s)
    {
        MAGICASSERT((fabs(s) > Epsilon), "Devidor is zero!");
        mX /= s;
        mY /= s;
        mZ /= s;
        return *this;
    }

    Vector3::~Vector3()
    {
    }
}
#include "Vector2.h"
#include <math.h>

namespace MagicDGP
{
    Vector2::Vector2() : 
        mX(0), 
        mY(0)
    {
    }

    Vector2::Vector2(Real x, Real y) : 
        mX(x), 
        mY(y)
    {
    }

    Real Vector2::Length() const
    {
        return sqrt(mX * mX + mY * mY);
    }

    Real Vector2::LengthSquared() const
    {
        return mX * mX + mY * mY;
    }

    Real Vector2::Distance(const Vector2& vec2) const
    {
        return (*this - vec2).Length();
    }

    Real Vector2::DistanceSquared(const Vector2& vec2) const
    {
        return (*this - vec2).LengthSquared();
    }

    Real Vector2::Normalise()
    {
        Real length = sqrt(mX * mX + mY * mY);
        if (length > Epsilon)
        {
            mX /= length;
            mY /= length;
        }
        return length;
    }

    Real Vector2::operator [] (int index) const
    {
        MAGICASSERT((index < 2 && index > -1), "Vector2 index out of range!");
        return *(&mX + index);
    }

    Real& Vector2::operator [] (int index)
    {
        MAGICASSERT((index < 2 && index > -1), "Vector2 index out of range!");
        return *(&mX + index);
    }
    bool Vector2::operator == (const Vector2& vec2) const
    {
        return (mX == vec2[0] && mY == vec2[1]);
    }

    Vector2 Vector2::operator + (const Vector2& vec2) const
    {
        return Vector2(mX + vec2[0], mY + vec2[1]);
    }

    Vector2 Vector2::operator - (const Vector2& vec2) const
    {
        return Vector2(mX - vec2[0], mY - vec2[1]);
    }

    Vector2 Vector2::operator * (Real s) const
    {
        return Vector2(mX * s, mY * s);
    }

    Real Vector2::operator * (const Vector2& vec2) const
    {
        return (mX * vec2[0] + mY * vec2[1]);
    }

    Vector2 Vector2::operator / (Real s) const
    {
        MAGICASSERT((fabs(s) > Epsilon), "Devidor is zero!");
        return Vector2(mX / s, mY / s);
    }

    Vector2 Vector2::operator - (void) const
    {
        return Vector2(-mX, -mY);
    }

    Vector2& Vector2::operator += (const Vector2& vec2)
    {
        mX += vec2[0];
        mY += vec2[1];
        return *this;
    }

    Vector2& Vector2::operator -= (const Vector2& vec2)
    {
        mX -= vec2[0];
        mY -= vec2[1];
        return *this;
    }

    Vector2& Vector2::operator *= (Real s)
    {
        mX *= s;
        mY *= s;
        return *this;
    }

    Vector2& Vector2::operator /= (Real s)
    {
        MAGICASSERT((fabs(s) > Epsilon), "Devidor is zero!");
        mX /= s;
        mY /= s;
        return *this;
    }

    Vector2::~Vector2()
    {
    }
}
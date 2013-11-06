#pragma once
#include "Vector3.h"
#include <vector>

namespace MagicDGP
{
    class Point3D
    {
    public:
        Point3D();
        Point3D(const Vector3& pos);
        Point3D(const Vector3& pos, const Vector3& nor);
        Point3D(const Vector3& pos, int id);
        Point3D(const Vector3& pos, const Vector3& nor, int id);
        ~Point3D();

        void SetPosition(const Vector3& pos);
        Vector3 GetPosition() const;
        void SetNormal(const Vector3& nor);
        Vector3 GetNormal() const;
        void SetColor(const Vector3& color);
        Vector3 GetColor() const;
        void SetId(int id);
        int GetId() const;
        void SetValid(bool valid);
        bool IsValid() const;

    protected:
        Vector3 mPosition;
        Vector3 mNormal;
        Vector3 mColor;
        int mId;
        bool mValid;
    };

    class Point3DSet
    {
    public:
        Point3DSet();
        ~Point3DSet();

        std::vector<Point3D* >& GetPointSet();
        Point3D* GetPoint(int index);
        const Point3D* GetPoint(int index) const;
        void SetPoint(Point3D* pPoint);
        void UnifyPosition(Real size);
        void InsertPoint(Point3D* pPoint);
        int  GetPointNumber() const;
        void SetColor(Vector3 color);
        void GetBBox(Vector3& bboxMin, Vector3& bboxMax) const;
        void CalculateBBox();
        Real GetDensity() const;
        void CalculateDensity();

    protected:
        std::vector<Point3D* > mPointSet;
        Vector3 mBBoxMin, mBBoxMax;
        Real mDensity;
    };

}
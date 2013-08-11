#pragma once
#include "Vector3.h"
#include <map>

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
        void SetId(int id);
        int GetId() const;

    protected:
        Vector3 mPosition;
        Vector3 mNormal;
        int mId;
    };

    class Point3DSet
    {
    public:
        Point3DSet();
        ~Point3DSet();

        std::map<int, Point3D* >& GetPointSet();
        Point3D* GetPoint(int index);
        void SetPoint(Point3D* pPoint);
        void UnifyPosition(Real size);
        bool InsertPoint(Point3D* pPoint);

    protected:
        std::map<int, Point3D* > mPointSet;
    };

}
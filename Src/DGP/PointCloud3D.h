#pragma once
#include "../Math/Vector3.h"
#include <vector>

namespace MagicDGP
{
    class Point3D
    {
    public:
        Point3D();
        Point3D(const MagicMath::Vector3& pos);
        Point3D(const MagicMath::Vector3& pos, const MagicMath::Vector3& nor);
        Point3D(const MagicMath::Vector3& pos, int id);
        Point3D(const MagicMath::Vector3& pos, const MagicMath::Vector3& nor, int id);
        ~Point3D();

        void SetPosition(const MagicMath::Vector3& pos);
        MagicMath::Vector3 GetPosition() const;
        void SetNormal(const MagicMath::Vector3& nor);
        MagicMath::Vector3 GetNormal() const;
        void SetColor(const MagicMath::Vector3& color);
        MagicMath::Vector3 GetColor() const;
        void SetId(int id);
        int GetId() const;
        void SetValid(bool valid);
        bool IsValid() const;

    protected:
        MagicMath::Vector3 mPosition;
        MagicMath::Vector3 mNormal;
        MagicMath::Vector3 mColor;
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
        bool SetPoint(int index, Point3D* pPoint);
        void UnifyPosition(double size);
        void InsertPoint(Point3D* pPoint);
        int  GetPointNumber() const;
        void SetColor(MagicMath::Vector3 color);
        void GetBBox(MagicMath::Vector3& bboxMin, MagicMath::Vector3& bboxMax) const;
        void CalculateBBox();
        double GetDensity() const;
        void CalculateDensity();
        bool HasNormal() const;
        void SetHasNormal(bool has);

    protected:
        std::vector<Point3D* > mPointSet;
        MagicMath::Vector3 mBBoxMin, mBBoxMax;
        double mDensity;
        bool mHasNormal;
    };

}
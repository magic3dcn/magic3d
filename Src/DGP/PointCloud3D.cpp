#include "StdAfx.h"
#include "PointCloud3D.h"

namespace MagicDGP
{
    Point3D::Point3D()
    {
    }

    Point3D::Point3D(const Vector3& pos) : mPosition(pos)
    {
    }

    Point3D::Point3D(const Vector3& pos, const Vector3& nor) : 
        mPosition(pos),
        mNormal(nor)
    {
    }

    Point3D::Point3D(const Vector3& pos, const Vector3& nor, int id) : 
        mPosition(pos),
        mNormal(nor),
        mId(id)
    {
    }

    Point3D::~Point3D()
    {
    }

    inline void Point3D::SetPosition(const Vector3& pos)
    {
        mPosition = pos;
    }

    inline Vector3 Point3D::GetPosition() const
    {
        return mPosition;
    }

    inline void Point3D::SetNormal(const Vector3& nor)
    {
        mNormal = nor;
    }

    inline Vector3 Point3D::GetNormal() const
    {
        return mNormal;
    }

    inline void Point3D::SetId(int id)
    {
        mId = id;
    }

    inline int Point3D::GetId() const
    {
        return mId;
    }

    Point3DSet::Point3DSet()
    {
    }

    Point3DSet::~Point3DSet()
    {
    }

    std::map<int, Point3D*>& Point3DSet::GetPointSet()
    {
        return mPointSet;
    }

    Point3D* Point3DSet::GetPoint(int index)
    {
        return mPointSet[index];
    }

    void Point3DSet::SetPoint(Point3D* pPoint)
    {
        mPointSet[pPoint->GetId()] = pPoint;
    }

    void Point3DSet::UnifyPosition(Real size)
    {
        Vector3 posMin(10e10, 10e10, 10e10);
        Vector3 posMax(-10e10, -10e10, -10e10);
        for (std::map<int, Point3D*>::iterator itr = mPointSet.begin(); itr != mPointSet.end(); ++itr)
        {
            Vector3 pos = itr->second->GetPosition();
            posMin[0] = posMin[0] < pos[0] ? posMin[0] : pos[0];
            posMin[1] = posMin[1] < pos[1] ? posMin[1] : pos[1];
            posMin[2] = posMin[2] < pos[2] ? posMin[2] : pos[2];
            posMax[0] = posMax[0] > pos[0] ? posMax[0] : pos[0];
            posMax[1] = posMax[1] > pos[1] ? posMax[1] : pos[1];
            posMax[2] = posMax[2] > pos[2] ? posMax[2] : pos[2];
        }
        Vector3 scale3 = posMax - posMin;
        Real scaleMax = scale3[0];
        if (scaleMax < scale3[1])
        {
            scaleMax = scale3[1];
        }
        if (scaleMax < scale3[2])
        {
            scaleMax = scale3[2];
        }
        if (scaleMax > Epsilon)
        {
            Real scaleV = size / scaleV;
            Vector3 centerPos = (posMin + posMax) / 2.0;
            for (std::map<int, Point3D*>::iterator itr = mPointSet.begin(); itr != mPointSet.end(); ++itr)
            {
                itr->second->SetPosition((itr->second->GetPosition() - centerPos) * scaleV);
            }
        }
    }

    bool Point3DSet::InsertPoint(Point3D* pPoint)
    {
        std::pair<std::map<int, Point3D* >::iterator, bool> ret = mPointSet.insert(std::make_pair(pPoint->GetId(), pPoint));
        return ret.second;
    }

}
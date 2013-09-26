//#include "StdAfx.h"
#include "PointCloud3D.h"

namespace MagicDGP
{
    Point3D::Point3D() : 
        mPosition(0, 0, 0),
        mNormal(0, 0, 0),
        mColor(1, 1, 0),
        mId(-1)
    {
    }

    Point3D::Point3D(const Vector3& pos) : 
        mPosition(pos),
        mNormal(0, 0, 0),
        mColor(1, 1, 0),
        mId(-1)
    {
    }

    Point3D::Point3D(const Vector3& pos, const Vector3& nor) : 
        mPosition(pos),
        mNormal(nor),
        mColor(1, 1, 0),
        mId(-1)
    {
    }

    Point3D::Point3D(const Vector3& pos, int id) :
        mPosition(pos),
        mNormal(0, 0, 0),
        mColor(1, 1, 0),
        mId(id)
    {
    }

    Point3D::Point3D(const Vector3& pos, const Vector3& nor, int id) : 
        mPosition(pos),
        mNormal(nor),
        mColor(1, 1, 0),
        mId(id)
    {
    }

    Point3D::~Point3D()
    {
    }

    void Point3D::SetPosition(const Vector3& pos)
    {
        mPosition = pos;
    }

    Vector3 Point3D::GetPosition() const
    {
        return mPosition;
    }

    void Point3D::SetNormal(const Vector3& nor)
    {
        mNormal = nor;
    }

    Vector3 Point3D::GetNormal() const
    {
        return mNormal;
    }

    void Point3D::SetColor(const Vector3& color)
    {
        mColor = color;
    }

    Vector3 Point3D::GetColor() const
    {
        return mColor;
    }

    void Point3D::SetId(int id)
    {
        mId = id;
    }

    int Point3D::GetId() const
    {
        return mId;
    }

    Point3DSet::Point3DSet()
    {
    }

    Point3DSet::~Point3DSet()
    {
        for (std::vector<Point3D* >::iterator itr = mPointSet.begin(); itr != mPointSet.end(); itr++)
        {
            if (*itr != NULL)
            {
                delete *itr;
                *itr = NULL;
            }
        }
        mPointSet.clear();
    }

    std::vector<Point3D* >& Point3DSet::GetPointSet()
    {
        return mPointSet;
    }

    Point3D* Point3DSet::GetPoint(int index)
    {
        return mPointSet.at(index);
    }

    const Point3D* Point3DSet::GetPoint(int index) const
    {
        return mPointSet.at(index);
    }

    void Point3DSet::SetPoint(Point3D* pPoint)
    {
        int index = pPoint->GetId();
        if (mPointSet.at(index) != NULL)
        {
            delete mPointSet[index];
        }
        mPointSet[index] = pPoint;
    }

    void Point3DSet::UnifyPosition(Real size)
    {
        Vector3 posMin(10e10, 10e10, 10e10);
        Vector3 posMax(-10e10, -10e10, -10e10);
        for (std::vector<Point3D* >::iterator itr = mPointSet.begin(); itr != mPointSet.end(); ++itr)
        {
            Vector3 pos = (*itr)->GetPosition();
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
            Real scaleV = size / scaleMax;
            Vector3 centerPos = (posMin + posMax) / 2.0;
            for (std::vector<Point3D* >::iterator itr = mPointSet.begin(); itr != mPointSet.end(); ++itr)
            {
                (*itr)->SetPosition(((*itr)->GetPosition() - centerPos) * scaleV);
            }
        }
        mBBoxMin = Vector3(-size, -size, -size);
        mBBoxMax = Vector3(size, size, size);
    }

    void Point3DSet::InsertPoint(Point3D* pPoint)
    {
        pPoint->SetId(mPointSet.size());
        mPointSet.push_back(pPoint);
    }

    int Point3DSet::GetPointNumber() const
    {
        return mPointSet.size();
    }

    void Point3DSet::SetColor(Vector3 color)
    {
        for (std::vector<Point3D* >::iterator itr = mPointSet.begin(); itr != mPointSet.end(); ++itr)
        {
            (*itr)->SetColor(color);
        }
    }

    void Point3DSet::CalculateBBox()
    {
    }

    void Point3DSet::GetBBox(Vector3& bboxMin, Vector3& bboxMax) const
    {
        bboxMin = mBBoxMin;
        bboxMax = mBBoxMax;
    }
}
//#include "StdAfx.h"
#include "PointCloud3D.h"
#include "flann/flann.h"

namespace MagicDGP
{
    Point3D::Point3D() : 
        mPosition(0, 0, 0),
        mNormal(0, 0, 0),
        mColor(0.09, 0.48627, 0.69),
        mId(-1), 
        mValid(true)
    {
    }

    Point3D::Point3D(const Vector3& pos) : 
        mPosition(pos),
        mNormal(0, 0, 0),
        mColor(0.09, 0.48627, 0.69),
        mId(-1),
        mValid(true)
    {
    }

    Point3D::Point3D(const Vector3& pos, const Vector3& nor) : 
        mPosition(pos),
        mNormal(nor),
        mColor(0.09, 0.48627, 0.69),
        mId(-1),
        mValid(true)
    {
    }

    Point3D::Point3D(const Vector3& pos, int id) :
        mPosition(pos),
        mNormal(0, 0, 0),
        mColor(0.09, 0.48627, 0.69),
        mId(id),
        mValid(true)
    {
    }

    Point3D::Point3D(const Vector3& pos, const Vector3& nor, int id) : 
        mPosition(pos),
        mNormal(nor),
        mColor(0.09, 0.48627, 0.69),
        mId(id),
        mValid(true)
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

    void Point3D::SetValid(bool valid)
    {
        mValid = valid;
    }

    bool Point3D::IsValid() const
    {
        return mValid;
    }

    Point3DSet::Point3DSet() : 
        mHasNormal(false)
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

    bool Point3DSet::SetPoint(int index, Point3D* pPoint)
    {
        if (index >=0 && index < mPointSet.size())
        {
            if (mPointSet.at(index) != NULL)
            {
                delete mPointSet[index];
            }
            mPointSet[index] = pPoint;
            return true;
        }
        else
        {
            return false;
        }
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
        Vector3 posTemp = mPointSet.at(0)->GetPosition();
        mBBoxMin[0] = mBBoxMax[0] = posTemp[0];
        mBBoxMin[1] = mBBoxMax[1] = posTemp[1];
        mBBoxMin[2] = mBBoxMax[2] = posTemp[2];
        int pointNum = mPointSet.size();
        for (int i = 0; i < pointNum; i++)
        {
            Vector3 pos = mPointSet.at(i)->GetPosition();
            for (int k = 0; k < 3; k++)
            {
                if (mBBoxMin[k] > pos[k])
                {
                    mBBoxMin[k] = pos[k];
                }
                if (mBBoxMax[k] < pos[k])
                {
                    mBBoxMax[k] = pos[k];
                }
            }
        }
        DebugLog << "BBoxMin: " << mBBoxMin[0] << " " << mBBoxMin[1] << " " << mBBoxMin[2] << " "
            << "BBoxMax: " << mBBoxMax[0] << " " << mBBoxMax[1] << " " << mBBoxMax[2] << std::endl;
    }

    void Point3DSet::GetBBox(Vector3& bboxMin, Vector3& bboxMax) const
    {
        bboxMin = mBBoxMin;
        bboxMax = mBBoxMax;
    }

    Real Point3DSet::GetDensity() const
    {
        return mDensity;
    }

    void Point3DSet::CalculateDensity()
    {
        int dim = 3;
        int pointNum = mPointSet.size();
        int refNum = pointNum;
        float* dataSet = new float[refNum * dim];
        int searchNum = pointNum;
        float* searchSet = new float[searchNum * dim];
        for (int i = 0; i < pointNum; i++)
        {
            Vector3 pos = mPointSet.at(i)->GetPosition();
            dataSet[dim * i + 0] = pos[0];
            dataSet[dim * i + 1] = pos[1];
            dataSet[dim * i + 2] = pos[2];
            searchSet[dim * i + 0] = pos[0];
            searchSet[dim * i + 1] = pos[1];
            searchSet[dim * i + 2] = pos[2];
        }
        int nn = 9;
        int* pIndex = new int[searchNum * nn];
        float* pDist = new float[searchNum * nn];
        FLANNParameters searchPara;
        searchPara = DEFAULT_FLANN_PARAMETERS;
        searchPara.algorithm = FLANN_INDEX_KDTREE;
        searchPara.trees = 8;
        searchPara.log_level = FLANN_LOG_INFO;
        searchPara.checks = 64;
        float speedup;
        flann_index_t indexId = flann_build_index(dataSet, refNum, dim, &speedup, &searchPara);
        flann_find_nearest_neighbors_index(indexId, searchSet, searchNum, pIndex, pDist, nn, &searchPara);
        flann_free_index(indexId, &searchPara);
        delete []dataSet;
        delete []searchSet;

        mDensity = 0;
        for (int i = 0; i < pointNum; i++)
        {
            int baseIndex = nn * i;
            for (int j = 0; j < nn; j++)
            {
                mDensity += pDist[baseIndex + j];
            }
        }
        mDensity /= (pointNum * nn);
        DebugLog << "Point Cloud Density: " << mDensity << std::endl;
        if (pIndex != NULL)
        {
            delete []pIndex;
            pIndex = NULL;
        }
        if (pDist != NULL)
        {
            delete []pDist;
            pDist = NULL;
        }
    }

    bool Point3DSet::HasNormal() const
    {
        return mHasNormal;
    }

    void Point3DSet::SetHasNormal(bool has)
    {
        mHasNormal = has;
    }
}
#include "StdAfx.h"
#include "SignedDistanceFunction.h"
#include "../Common/LogSystem.h"
#include "Parser.h"

namespace MagicDGP
{
    SignedDistanceFunction::SignedDistanceFunction(int resX, int resY, int resZ, float minX, float maxX, float minY, float maxY, float minZ, float maxZ) :
        mSDF(resX * resY * resZ, 0.f),
        mWeight(resX * resY * resZ, 0.f),
        mResolutionX(resX),
        mResolutionY(resY),
        mResolutionZ(resZ),
        mMinX(minX),
        mMinY(minY),
        mMinZ(minZ),
        mMaxX(maxX),
        mMaxY(maxY),
        mMaxZ(maxZ)
    {

    }

    SignedDistanceFunction::~SignedDistanceFunction()
    {
    }

    void SignedDistanceFunction::UpdateSDF(const Point3DSet* pPC, const HomoMatrix4* pTransform)
    {
        MagicLog << "SignedDistanceFunction::UpdateSDF" << std::endl;
        int pcNum = pPC->GetPointNumber();
        float deltaX = (mMaxX - mMinX) / mResolutionX;
        float deltaY = (mMaxY - mMinY) / mResolutionY;
        float deltaZ = (mMaxZ - mMinZ) / mResolutionZ;
        int truncW = 3;
        for (int i = 0; i < pcNum; i++)
        {
            Vector3 pos = pPC->GetPoint(i)->GetPosition();
            if (pos[0] < mMinX || pos[0] > mMaxX 
                || pos[1] < mMinY || pos[1] > mMaxY 
                || pos[2] < mMinZ || pos[2] > mMaxZ)
            {
                continue;
            }
            int   xIndex = (pos[0] - mMinX) / deltaX;
            int   yIndex = (pos[1] - mMinY) / deltaY;
            int   zIndex = (pos[2] - mMinZ) / deltaZ;
            Vector3 pixelPos(mMinX + xIndex * deltaX, mMinY + yIndex * deltaY, mMinZ + zIndex * deltaZ);
            Vector3 pos_trans = pTransform->TransformPoint(pos); //TransformPosition(pos, pTransform);
            int xZero = (pos_trans[0] - mMinX) / deltaX;
            int yZero = (pos_trans[1] - mMinY) / deltaY;
            int zZero = (pos_trans[2] - mMinZ) / deltaZ;
            int indexZero = xZero * mResolutionY * mResolutionZ + yZero * mResolutionZ + zZero;
            mSDF.at(indexZero) = mSDF.at(indexZero) * mWeight.at(indexZero) / (mWeight.at(indexZero) + 1.f);
            mWeight.at(indexZero) += 1.f;
            mPCIndex.insert(indexZero);
            //
            for (int j = 1; j <= truncW; j++)
            {
                Vector3 posPosit(pixelPos[0], pixelPos[1], pixelPos[2] + deltaZ * j);
                Vector3 posPosit_trans = pTransform->TransformPoint(posPosit);  //TransformPosition(posPosit, pTransform);
                float distPosit = (posPosit_trans - pos_trans).Length();
                int xPosit = (posPosit_trans[0] - mMinX) / deltaX;
                int yPosit = (posPosit_trans[1] - mMinY) / deltaY;
                int zPosit = (posPosit_trans[2] - mMinZ) / deltaZ;
                //need to judge the range
                int indexPosit = xPosit * mResolutionY * mResolutionZ + yPosit * mResolutionZ + zPosit;
                mSDF.at(indexPosit) = (mSDF.at(indexPosit) * mWeight.at(indexPosit) + distPosit)
                    / (mWeight.at(indexPosit) + 1.f);
                mWeight.at(indexPosit) += 1.f;
                mPCIndex.insert(indexPosit);

                Vector3 posNegat(pixelPos[0], pixelPos[1], pixelPos[2] - deltaZ * j);
                Vector3 posNegat_trans = pTransform->TransformPoint(posNegat); //TransformPosition(posNegat, pTransform);
                float distNegat = -1.f * (posNegat_trans - pos_trans).Length();
                int xNegat = (posNegat_trans[0] - mMinX) / deltaX;
                int yNegat = (posNegat_trans[1] - mMinY) / deltaY;
                int zNegat = (posNegat_trans[2] - mMinZ) / deltaZ;
                //need to judge the range
                int indexNegat = xNegat * mResolutionY * mResolutionZ + yNegat * mResolutionZ + zNegat;
                mSDF.at(indexNegat) = (mSDF.at(indexNegat) * mWeight.at(indexNegat) + distNegat)
                    / (mWeight.at(indexNegat) + 1.f);
                mWeight.at(indexNegat) += 1.f;
                mPCIndex.insert(indexNegat);
            }
        }
        MagicLog << "PC index: " << mPCIndex.size() << std::endl;
    }

    Point3DSet* SignedDistanceFunction::ExtractPointCloud()
    {
        MagicLog << "SignedDistanceFunction::PointCloudPrediction" << std::endl;
        float deltaX = (mMaxX - mMinX) / mResolutionX;
        float deltaY = (mMaxY - mMinY) / mResolutionY;
        float deltaZ = (mMaxZ - mMinZ) / mResolutionZ;
        std::vector<Vector3> posList;
        std::vector<Vector3> norList;
        int xGridNum = mResolutionY * mResolutionZ;
        int gridMaxNum = mResolutionX * mResolutionY * mResolutionZ;
        for (std::set<int>::iterator itr = mPCIndex.begin(); itr != mPCIndex.end(); itr++)
        {
            int xIndex = *itr / xGridNum;
            int yIndex = (*itr - xIndex * xGridNum) / mResolutionZ;
            int zIndex = *itr - xIndex * xGridNum - yIndex * mResolutionZ;
            float sdfOrigin = mSDF.at(*itr);
            //if it is a zero
            if (fabs(sdfOrigin) < Epsilon)
            {
                Vector3 posZero(mMinX + xIndex * deltaX, mMinY + yIndex * deltaY, mMinZ + zIndex * deltaZ);
                Vector3 norZero;
                //need to judge range
                norZero[0] = mSDF.at(*itr - xGridNum) - mSDF.at(*itr + xGridNum);
                norZero[1] = mSDF.at(*itr - mResolutionZ) - mSDF.at(*itr + mResolutionZ);
                norZero[2] = mSDF.at(*itr - 1) - mSDF.at(*itr + 1);
                float norLen = norZero.Normalise();
                if (norLen > Epsilon)
                {
                    posList.push_back(posZero);
                    norList.push_back(norZero);
                }
                else
                {
                   // MagicLog << "Normal Zero" << std::endl;
                }
                continue;
            }
            //
            //x direction
            int xDirIndex = *itr + xGridNum;
            if (xDirIndex < gridMaxNum)
            {
                float sdfX = mSDF.at(xDirIndex);
                if (sdfX * sdfOrigin < 0.f)
                {
                    //Extract a point at the middle
                    Vector3 posX(mMinX + (xIndex * 2.f + 1.f) * deltaX / 2.f,
                        mMinY + yIndex * deltaY, mMinZ + zIndex * deltaZ);
                    Vector3 norX;
                    norX[0] = mSDF.at(*itr) - mSDF.at(*itr + xGridNum);
                    if (*itr + mResolutionZ < gridMaxNum)
                    {
                        norX[1] = mSDF.at(*itr) - mSDF.at(*itr + mResolutionZ);
                    }
                    else
                    {
                        norX[1] = mSDF.at(*itr - mResolutionZ) - mSDF.at(*itr);
                    }
                    if (*itr + 1 < gridMaxNum)
                    {
                        norX[2] = mSDF.at(*itr) - mSDF.at(*itr + 1);
                    }
                    else
                    {
                        norX[2] = mSDF.at(*itr - 1) - mSDF.at(*itr);
                    }
                    float norLen = norX.Normalise();
                    if (norLen > Epsilon)
                    {
                        posList.push_back(posX);
                        norList.push_back(norX);
                    }
                    else
                    {
                        MagicLog << "Normal Zero" << std::endl;
                    }
                }
            }
            //y direction
            int yDirIndex = *itr + mResolutionZ;
            if (yDirIndex < gridMaxNum)
            {
                float sdfY = mSDF.at(yDirIndex);
                if (sdfY * sdfOrigin < 0.f)
                {
                    //Extract a point at the middle
                    Vector3 posY(mMinY + xIndex * deltaX,
                        mMinY + (yIndex * 2.f + 1.f) * deltaY / 2.f, mMinZ + zIndex * deltaZ);
                    Vector3 norY;
                    if (*itr + xGridNum < gridMaxNum)
                    {
                        norY[0] = mSDF.at(*itr) - mSDF.at(*itr + xGridNum);
                    }
                    else
                    {
                        norY[0] = mSDF.at(*itr - xGridNum) - mSDF.at(*itr);
                    }
                    norY[1] = mSDF.at(*itr) - mSDF.at(*itr + mResolutionZ);
                    if (*itr + 1 < gridMaxNum)
                    {
                        norY[2] = mSDF.at(*itr) - mSDF.at(*itr + 1);
                    }
                    else
                    {
                        norY[2] = mSDF.at(*itr - 1) - mSDF.at(*itr);
                    }
                    float norLen = norY.Normalise();
                    if (norLen > Epsilon)
                    {
                        posList.push_back(posY);
                        norList.push_back(norY);
                    }
                    else
                    {
                        MagicLog << "Normal Zero" << std::endl;
                    }
                }
            }
            //z direction
            int zDirIndex = *itr + 1;
            if (zDirIndex < gridMaxNum)
            {
                float sdfZ = mSDF.at(zDirIndex);
                if (sdfZ * sdfOrigin < 0.f)
                {
                    //Extract a point at the middle
                    Vector3 posZ(mMinX + xIndex * deltaX,
                        mMinY + yIndex * deltaY, mMinZ + (zIndex * 2.f + 1.f) * deltaZ / 2.f);
                    Vector3 norZ;
                    if (*itr + xGridNum < gridMaxNum)
                    {
                        norZ[0] = mSDF.at(*itr) - mSDF.at(*itr + xGridNum);
                    }
                    else
                    {
                        norZ[0] = mSDF.at(*itr - xGridNum) - mSDF.at(*itr);
                    }
                    if (*itr + mResolutionZ < gridMaxNum)
                    {
                        norZ[1] = mSDF.at(*itr) - mSDF.at(*itr + mResolutionZ);
                    }
                    else
                    {
                        norZ[1] = mSDF.at(*itr - mResolutionZ) - mSDF.at(*itr);
                    }
                    norZ[2] = mSDF.at(*itr) - mSDF.at(*itr + 1);
                    float norLen = norZ.Normalise();
                    if (norLen > Epsilon)
                    {
                        posList.push_back(posZ);
                        norList.push_back(norZ);
                    }
                    else
                    {
                        MagicLog << "Normal Zero" << std::endl;
                    }
                }
            }
        }

        int posNum = posList.size();
        MagicLog << "Export point size: " << posNum << std::endl;
        if (posNum > 0)
        {
            Point3DSet* pPC = new Point3DSet;
            for (int i = 0; i < posNum; i++)
            {
                Point3D* point = new Point3D(posList.at(i), norList.at(i));
                pPC->InsertPoint(point);
            }
            return pPC;
        }
        else
        {
            return NULL;
        }
    }

}
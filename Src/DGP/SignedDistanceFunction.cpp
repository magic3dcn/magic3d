//#include "StdAfx.h"
#include "SignedDistanceFunction.h"
#include "Tool/LogSystem.h"
#include "Parser.h"

namespace MagicDGP
{
    SignedDistanceFunction::SignedDistanceFunction(int resX, int resY, int resZ, float minX, float maxX, float minY, float maxY, float minZ, float maxZ) :
        mSDF((resX + 1) * (resY + 1) * (resZ + 1), 0.f),
        mWeight((resX + 1) * (resY + 1) * (resZ + 1), 0.f),
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

    void SignedDistanceFunction::UpdateSDF(const Point3DSet* pPC, const MagicMath::HomoMatrix4* pTransform)
    {
        //DebugLog << "SignedDistanceFunction::UpdateSDF" << std::endl;
        int pcNum = pPC->GetPointNumber();
        float deltaX = (mMaxX - mMinX) / mResolutionX;
        float deltaY = (mMaxY - mMinY) / mResolutionY;
        float deltaZ = (mMaxZ - mMinZ) / mResolutionZ;
        int truncW = 5;
        int maxIndex = (mResolutionX + 1) * (mResolutionY + 1) * (mResolutionZ + 1) - 1;
        for (int i = 0; i < pcNum; i++)
        {
            MagicMath::Vector3 pos = pPC->GetPoint(i)->GetPosition();
            if (pos[0] < mMinX || pos[0] > mMaxX 
                || pos[1] < mMinY || pos[1] > mMaxY 
                || pos[2] < mMinZ || pos[2] > mMaxZ)
            {
                continue;
            }
            int   xIndex = (pos[0] - mMinX) / deltaX;
            int   yIndex = (pos[1] - mMinY) / deltaY;
            int   zIndex = (pos[2] - mMinZ) / deltaZ;///////////
            MagicMath::Vector3 pixelPos(mMinX + xIndex * deltaX, mMinY + yIndex * deltaY, mMinZ + zIndex * deltaZ);
            MagicMath::Vector3 pos_trans = pTransform->TransformPoint(pos); //TransformPosition(pos, pTransform);
            int xZero = (pos_trans[0] - mMinX) / deltaX;
            int yZero = (pos_trans[1] - mMinY) / deltaY;
            int zZero = (pos_trans[2] - mMinZ) / deltaZ;
            if (xZero < 0 || yZero < 0 || zZero < 0)
            {
                continue;
            }
            int indexZero = xZero * mResolutionY * mResolutionZ + yZero * mResolutionZ + zZero;
            if (indexZero > maxIndex)
            {
                continue;
            }
            mSDF.at(indexZero) = mSDF.at(indexZero) * mWeight.at(indexZero) / (mWeight.at(indexZero) + 1.f);
            mWeight.at(indexZero) += 1.f;
            mPCIndex.insert(indexZero);
            //
            for (int j = 1; j <= truncW; j++)
            {
                //MagicMath::Vector3 posPosit(pixelPos[0], pixelPos[1], pixelPos[2] + deltaZ * j);
                MagicMath::Vector3 posPosit(pos[0], pos[1], pos[2] + deltaZ * j);
                MagicMath::Vector3 posPosit_trans = pTransform->TransformPoint(posPosit);  //TransformPosition(posPosit, pTransform);
                //float distPosit = (posPosit_trans - pos_trans).Length();
                int xPosit = (posPosit_trans[0] - mMinX) / deltaX;
                int yPosit = (posPosit_trans[1] - mMinY) / deltaY;
                int zPosit = (posPosit_trans[2] - mMinZ) / deltaZ;
                if (xPosit > 0 && yPosit > 0 && zPosit > 0)
                {
                    MagicMath::Vector3 deltaPosit((xPosit - xZero) * deltaX, (yPosit - yZero) * deltaY, (zPosit - zZero) * deltaZ);
                    float distPosit = deltaPosit.Length();
                    if (distPosit > 1.0e-15)
                    {
                        //need to judge the range
                        int indexPosit = xPosit * mResolutionY * mResolutionZ + yPosit * mResolutionZ + zPosit;
                        if (indexPosit < maxIndex)
                        {
                            mSDF.at(indexPosit) = (mSDF.at(indexPosit) * mWeight.at(indexPosit) + distPosit)
                                                / (mWeight.at(indexPosit) + 1.f);
                            //mSDF.at(indexPosit) = (mSDF.at(indexPosit) * mWeight.at(indexPosit) - distPosit)
                            //                    / (mWeight.at(indexPosit) + 1.f);
                            mWeight.at(indexPosit) += 1.f;
                            mPCIndex.insert(indexPosit);
                        }
                    }
                }

                MagicMath::Vector3 posNegat(pos[0], pos[1], pos[2] - deltaZ * j);
                MagicMath::Vector3 posNegat_trans = pTransform->TransformPoint(posNegat); //TransformPosition(posNegat, pTransform);
                //float distNegat = -1.f * (posNegat_trans - pos_trans).Length();
                int xNegat = (posNegat_trans[0] - mMinX) / deltaX;
                int yNegat = (posNegat_trans[1] - mMinY) / deltaY;
                int zNegat = (posNegat_trans[2] - mMinZ) / deltaZ;
                if (xNegat > 0 && yNegat > 0 && zNegat > 0)
                {
                    MagicMath::Vector3 deltaNegat((xNegat - xZero) * deltaX, (yNegat - yZero) * deltaY, (zNegat - zZero) * deltaZ);
                    float distNegat = deltaNegat.Length();
                    if (distNegat > 1.0e-15)
                    {
                        //need to judge the range
                        int indexNegat = xNegat * mResolutionY * mResolutionZ + yNegat * mResolutionZ + zNegat;
                        if (indexNegat < maxIndex)
                        {
                             mSDF.at(indexNegat) = (mSDF.at(indexNegat) * mWeight.at(indexNegat) - distNegat)
                                                / (mWeight.at(indexNegat) + 1.f);
                            //mSDF.at(indexNegat) = (mSDF.at(indexNegat) * mWeight.at(indexNegat) + distNegat)
                            //                    / (mWeight.at(indexNegat) + 1.f);
                            mWeight.at(indexNegat) += 1.f;
                            mPCIndex.insert(indexNegat);
                        }
                    }
                }
            }
        }
        //DebugLog << "PC index: " << mPCIndex.size() << std::endl;
    }

    void SignedDistanceFunction::UpdateFineSDF(const Point3DSet* pPC, const MagicMath::HomoMatrix4* pTransform)
    {
        DebugLog << "SignedDistanceFunction::UpdateFineSDF" << std::endl;
        int pcNum = pPC->GetPointNumber();
        float deltaX = (mMaxX - mMinX) / mResolutionX;
        float deltaY = (mMaxY - mMinY) / mResolutionY;
        float deltaZ = (mMaxZ - mMinZ) / mResolutionZ;
        int truncW = 3;
        int maxIndex = (mResolutionX + 1) * (mResolutionY + 1) * (mResolutionZ + 1) - 1;
        for (int i = 0; i < pcNum; i++)
        {
            MagicMath::Vector3 pos = pPC->GetPoint(i)->GetPosition();
            if (pos[0] < mMinX || pos[0] > mMaxX 
                || pos[1] < mMinY || pos[1] > mMaxY 
                || pos[2] < mMinZ || pos[2] > mMaxZ)
            {
                continue;
            }
            //int   xIndex = (pos[0] - mMinX) / deltaX;
            //int   yIndex = (pos[1] - mMinY) / deltaY;
            //int   zIndex = (pos[2] - mMinZ) / deltaZ;///////////
            //MagicMath::Vector3 pixelPos(mMinX + xIndex * deltaX, mMinY + yIndex * deltaY, mMinZ + zIndex * deltaZ);
            MagicMath::Vector3 pos_trans = pTransform->TransformPoint(pos); //TransformPosition(pos, pTransform);
            int xZero = (pos_trans[0] - mMinX) / deltaX;
            int yZero = (pos_trans[1] - mMinY) / deltaY;
            int zZero = (pos_trans[2] - mMinZ) / deltaZ;
            if (xZero < 0 || yZero < 0 || zZero < 0)
            {
                continue;
            }
            int indexZero = xZero * mResolutionY * mResolutionZ + yZero * mResolutionZ + zZero;
            if (indexZero > maxIndex)
            {
                continue;
            }
            mSDF.at(indexZero) = mSDF.at(indexZero) * mWeight.at(indexZero) / (mWeight.at(indexZero) + 1.f);
            mWeight.at(indexZero) += 1.f;
            mPCIndex.insert(indexZero);
            //
            for (int j = 1; j <= truncW; j++)
            {
                //MagicMath::Vector3 posPosit(pixelPos[0], pixelPos[1], pixelPos[2] + deltaZ * j);
                MagicMath::Vector3 posPosit(pos[0], pos[1], pos[2] + deltaZ * j);
                MagicMath::Vector3 posPosit_trans = pTransform->TransformPoint(posPosit);  //TransformPosition(posPosit, pTransform);
                //float distPosit = (posPosit_trans - pos_trans).Length();
                int xPosit = (posPosit_trans[0] - mMinX) / deltaX;
                int yPosit = (posPosit_trans[1] - mMinY) / deltaY;
                int zPosit = (posPosit_trans[2] - mMinZ) / deltaZ;
                if (xPosit > 0 && yPosit > 0 && zPosit > 0)
                {
                    MagicMath::Vector3 deltaPosit((xPosit - xZero) * deltaX, (yPosit - yZero) * deltaY, (zPosit - zZero) * deltaZ);
                    MagicMath::Vector3 pixelPosPosit(mMinX + deltaX * xPosit, mMinY + deltaY * yPosit, mMinZ + deltaZ * zPosit);
                    MagicMath::Vector3 distDeltaPosit = pixelPosPosit - pos_trans;
                    float distPosit = deltaPosit.Length();
                    if (distPosit > 1.0e-15)
                    {
                        //need to judge the range
                        int indexPosit = xPosit * mResolutionY * mResolutionZ + yPosit * mResolutionZ + zPosit;
                        if (indexPosit < maxIndex)
                        {
                            mSDF.at(indexPosit) = (mSDF.at(indexPosit) * mWeight.at(indexPosit) + distDeltaPosit.Length())
                                                / (mWeight.at(indexPosit) + 1.f);
                            //mSDF.at(indexPosit) = (mSDF.at(indexPosit) * mWeight.at(indexPosit) - distPosit)
                            //                    / (mWeight.at(indexPosit) + 1.f);
                            mWeight.at(indexPosit) += 1.f;
                            mPCIndex.insert(indexPosit);
                        }
                    }
                }

                MagicMath::Vector3 posNegat(pos[0], pos[1], pos[2] - deltaZ * j);
                MagicMath::Vector3 posNegat_trans = pTransform->TransformPoint(posNegat); //TransformPosition(posNegat, pTransform);
                //float distNegat = -1.f * (posNegat_trans - pos_trans).Length();
                int xNegat = (posNegat_trans[0] - mMinX) / deltaX;
                int yNegat = (posNegat_trans[1] - mMinY) / deltaY;
                int zNegat = (posNegat_trans[2] - mMinZ) / deltaZ;
                if (xNegat > 0 && yNegat > 0 && zNegat > 0)
                {
                    MagicMath::Vector3 deltaNegat((xNegat - xZero) * deltaX, (yNegat - yZero) * deltaY, (zNegat - zZero) * deltaZ);
                    MagicMath::Vector3 pixelPosNeget(mMinX + deltaX * xNegat, mMinY + deltaY * yNegat, mMinZ + deltaZ * zNegat);
                    MagicMath::Vector3 distDeltaNeget = pixelPosNeget - pos_trans;
                    float distNegat = deltaNegat.Length();
                    if (distNegat > 1.0e-15)
                    {
                        //need to judge the range
                        int indexNegat = xNegat * mResolutionY * mResolutionZ + yNegat * mResolutionZ + zNegat;
                        if (indexNegat < maxIndex)
                        {
                            mSDF.at(indexNegat) = (mSDF.at(indexNegat) * mWeight.at(indexNegat) - distDeltaNeget.Length())
                                                / (mWeight.at(indexNegat) + 1.f);
                            //mSDF.at(indexNegat) = (mSDF.at(indexNegat) * mWeight.at(indexNegat) + distNegat)
                            //                    / (mWeight.at(indexNegat) + 1.f);
                            mWeight.at(indexNegat) += 1.f;
                            mPCIndex.insert(indexNegat);
                        }
                    }
                }
            }
        }
        DebugLog << "PC index: " << mPCIndex.size() << std::endl;
    }

    Point3DSet* SignedDistanceFunction::ExtractPointCloud()
    {
        DebugLog << "SignedDistanceFunction::PointCloudPrediction" << std::endl;
        float deltaX = (mMaxX - mMinX) / mResolutionX;
        float deltaY = (mMaxY - mMinY) / mResolutionY;
        float deltaZ = (mMaxZ - mMinZ) / mResolutionZ;
        std::vector<MagicMath::Vector3> posList;
        std::vector<MagicMath::Vector3> norList;
        int xGridNum = mResolutionY * mResolutionZ;
        int gridMaxNum = (mResolutionX + 1) * (mResolutionY + 1) * (mResolutionZ + 1) - 1;
        for (std::set<int>::iterator itr = mPCIndex.begin(); itr != mPCIndex.end(); itr++)
        {
            int xIndex = *itr / xGridNum;
            int yIndex = (*itr - xIndex * xGridNum) / mResolutionZ;
            int zIndex = *itr - xIndex * xGridNum - yIndex * mResolutionZ;
            if (xIndex < 0 || xIndex > mResolutionX || yIndex < 0 || yIndex > mResolutionY || zIndex < 0 || zIndex > mResolutionZ)
            {
                continue;
            }
            float sdfOrigin = mSDF.at(*itr);
            //if it is a zero
            if (fabs(sdfOrigin) < 1.0e-15)
            {
                MagicMath::Vector3 posZero(mMinX + xIndex * deltaX, mMinY + yIndex * deltaY, mMinZ + zIndex * deltaZ);
                MagicMath::Vector3 norZero;
                //need to judge range
                if ((*itr - xGridNum) < 0 || (*itr + xGridNum) > gridMaxNum ||
                    (*itr - mResolutionZ) < 0 || (*itr + mResolutionZ) > gridMaxNum ||
                    (*itr - 1) < 0 || (*itr + 1) > gridMaxNum)
                {
                    continue;
                }
                //norZero[0] = mSDF.at(*itr - xGridNum) - mSDF.at(*itr + xGridNum);
                norZero[0] = mSDF.at(*itr + xGridNum) - mSDF.at(*itr - xGridNum);
                //norZero[1] = mSDF.at(*itr - mResolutionZ) - mSDF.at(*itr + mResolutionZ);
                norZero[1] = mSDF.at(*itr + mResolutionZ) - mSDF.at(*itr - mResolutionZ);
                //norZero[2] = mSDF.at(*itr - 1) - mSDF.at(*itr + 1);
                norZero[2] = mSDF.at(*itr + 1) - mSDF.at(*itr - 1);
                float norLen = norZero.Normalise();
                if (norLen > 1.0e-15)
                {
                    posList.push_back(posZero);
                    norList.push_back(norZero);
                }
                else
                {
                   // DebugLog << "Normal Zero: Zero" << std::endl;
                    //norZero[2] = -1.f;
                    //posList.push_back(posZero);
                    //norList.push_back(norZero);
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
                    MagicMath::Vector3 posX(mMinX + (xIndex * 2.f + 1.f) * deltaX / 2.f,
                        mMinY + yIndex * deltaY, mMinZ + zIndex * deltaZ);
                    MagicMath::Vector3 norX;
                    //norX[0] = mSDF.at(*itr) - mSDF.at(*itr + xGridNum);
                    norX[0] = mSDF.at(*itr + xGridNum) - mSDF.at(*itr);
                    if (*itr + mResolutionZ < gridMaxNum)
                    {
                        //norX[1] = mSDF.at(*itr) - mSDF.at(*itr + mResolutionZ);
                        norX[1] = mSDF.at(*itr + mResolutionZ) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norX[1] = mSDF.at(*itr - mResolutionZ) - mSDF.at(*itr);
                        norX[1] = mSDF.at(*itr) - mSDF.at(*itr - mResolutionZ);
                    }
                    if (*itr + 1 < gridMaxNum)
                    {
                        //norX[2] = mSDF.at(*itr) - mSDF.at(*itr + 1);
                        norX[2] = mSDF.at(*itr + 1) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norX[2] = mSDF.at(*itr - 1) - mSDF.at(*itr);
                        norX[2] = mSDF.at(*itr) - mSDF.at(*itr - 1);
                    }
                    float norLen = norX.Normalise();
                    if (norLen > 1.0e-15)
                    {
                        posList.push_back(posX);
                        norList.push_back(norX);
                    }
                    else
                    {
                        DebugLog << "Normal Zero: X Direction" << std::endl;
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
                    MagicMath::Vector3 posY(mMinX + xIndex * deltaX,
                        mMinY + (yIndex * 2.f + 1.f) * deltaY / 2.f, mMinZ + zIndex * deltaZ);
                    MagicMath::Vector3 norY;
                    if (*itr + xGridNum < gridMaxNum)
                    {
                        //norY[0] = mSDF.at(*itr) - mSDF.at(*itr + xGridNum);
                        norY[0] = mSDF.at(*itr + xGridNum) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norY[0] = mSDF.at(*itr - xGridNum) - mSDF.at(*itr);
                        norY[0] = mSDF.at(*itr) - mSDF.at(*itr - xGridNum);
                    }
                    //norY[1] = mSDF.at(*itr) - mSDF.at(*itr + mResolutionZ);
                    norY[1] = mSDF.at(*itr + mResolutionZ) - mSDF.at(*itr);
                    if (*itr + 1 < gridMaxNum)
                    {
                        //norY[2] = mSDF.at(*itr) - mSDF.at(*itr + 1);
                        norY[2] = mSDF.at(*itr + 1) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norY[2] = mSDF.at(*itr - 1) - mSDF.at(*itr);
                        norY[2] = mSDF.at(*itr) - mSDF.at(*itr - 1);
                    }
                    float norLen = norY.Normalise();
                    if (norLen > 1.0e-15)
                    {
                        posList.push_back(posY);
                        norList.push_back(norY);
                    }
                    else
                    {
                        DebugLog << "Normal Zero: Y direction" << std::endl;
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
                    MagicMath::Vector3 posZ(mMinX + xIndex * deltaX,
                        mMinY + yIndex * deltaY, mMinZ + (zIndex * 2.f + 1.f) * deltaZ / 2.f);
                    MagicMath::Vector3 norZ;
                    if (*itr + xGridNum < gridMaxNum)
                    {
                        //norZ[0] = mSDF.at(*itr) - mSDF.at(*itr + xGridNum);
                        norZ[0] = mSDF.at(*itr + xGridNum) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norZ[0] = mSDF.at(*itr - xGridNum) - mSDF.at(*itr);
                        norZ[0] = mSDF.at(*itr) - mSDF.at(*itr - xGridNum);
                    }
                    if (*itr + mResolutionZ < gridMaxNum)
                    {
                        //norZ[1] = mSDF.at(*itr) - mSDF.at(*itr + mResolutionZ);
                        norZ[1] = mSDF.at(*itr + mResolutionZ) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norZ[1] = mSDF.at(*itr - mResolutionZ) - mSDF.at(*itr);
                        norZ[1] = mSDF.at(*itr) - mSDF.at(*itr - mResolutionZ);
                    }
                    //norZ[2] = mSDF.at(*itr) - mSDF.at(*itr + 1);
                    norZ[2] = mSDF.at(*itr + 1) - mSDF.at(*itr);

                    float norLen = norZ.Normalise();
                    if (norLen > 1.0e-15)
                    {
                        posList.push_back(posZ);
                        norList.push_back(norZ);
                    }
                    else
                    {
                        DebugLog << "Normal Zero: Z Direction" << std::endl;
                    }
                }
            }
        }

        int posNum = posList.size();
        DebugLog << "Export point size: " << posNum << std::endl;
        if (posNum > 0)
        {
            Point3DSet* pPC = new Point3DSet;
            for (int i = 0; i < posNum; i++)
            {
                Point3D* point = new Point3D(posList.at(i), norList.at(i));
                pPC->InsertPoint(point);
            }
            //MagicDGP::Parser::ExportPointSet("interPointset.obj", pPC);//
            return pPC;
        }
        else
        {
            return NULL;
        }
    }

    Point3DSet* SignedDistanceFunction::ExtractFinePointCloud()
    {
        //DebugLog << "SignedDistanceFunction::ExtractFinePointCloud" << std::endl;
        float deltaX = (mMaxX - mMinX) / mResolutionX;
        float deltaY = (mMaxY - mMinY) / mResolutionY;
        float deltaZ = (mMaxZ - mMinZ) / mResolutionZ;
        std::vector<MagicMath::Vector3> posList;
        std::vector<MagicMath::Vector3> norList;
        int xGridNum = mResolutionY * mResolutionZ;
        int gridMaxNum = (mResolutionX + 1) * (mResolutionY + 1) * (mResolutionZ + 1) - 1;
        for (std::set<int>::iterator itr = mPCIndex.begin(); itr != mPCIndex.end(); itr++)
        {
            int xIndex = *itr / xGridNum;
            int yIndex = (*itr - xIndex * xGridNum) / mResolutionZ;
            int zIndex = *itr - xIndex * xGridNum - yIndex * mResolutionZ;
            if (xIndex < 0 || xIndex > mResolutionX || yIndex < 0 || yIndex > mResolutionY || zIndex < 0 || zIndex > mResolutionZ)
            {
                continue;
            }
            float sdfOrigin = mSDF.at(*itr);
            //if it is a zero
            if (fabs(sdfOrigin) < 1.0e-15)
            {
                MagicMath::Vector3 posZero(mMinX + xIndex * deltaX, mMinY + yIndex * deltaY, mMinZ + zIndex * deltaZ);
                MagicMath::Vector3 norZero;
                //need to judge range
                if ((*itr - xGridNum) < 0 || (*itr + xGridNum) > gridMaxNum ||
                    (*itr - mResolutionZ) < 0 || (*itr + mResolutionZ) > gridMaxNum ||
                    (*itr - 1) < 0 || (*itr + 1) > gridMaxNum)
                {
                    continue;
                }
                //norZero[0] = mSDF.at(*itr - xGridNum) - mSDF.at(*itr + xGridNum);
                norZero[0] = mSDF.at(*itr + xGridNum) - mSDF.at(*itr - xGridNum);
                //norZero[1] = mSDF.at(*itr - mResolutionZ) - mSDF.at(*itr + mResolutionZ);
                norZero[1] = mSDF.at(*itr + mResolutionZ) - mSDF.at(*itr - mResolutionZ);
                //norZero[2] = mSDF.at(*itr - 1) - mSDF.at(*itr + 1);
                norZero[2] = mSDF.at(*itr + 1) - mSDF.at(*itr - 1);
                float norLen = norZero.Normalise();
                if (norLen > 1.0e-15)
                {
                    posList.push_back(posZero);
                    norList.push_back(norZero);
                }
                else
                {
                   // DebugLog << "Normal Zero: Zero" << std::endl;
                    //norZero[2] = -1.f;
                    //posList.push_back(posZero);
                    //norList.push_back(norZero);
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
                    float wr = fabs(sdfOrigin / (sdfOrigin - sdfX));
                    MagicMath::Vector3 posX(mMinX + (xIndex + wr) * deltaX,
                        mMinY + yIndex * deltaY, mMinZ + zIndex * deltaZ);
                    MagicMath::Vector3 norX;
                    //norX[0] = mSDF.at(*itr) - mSDF.at(*itr + xGridNum);
                    norX[0] = mSDF.at(*itr + xGridNum) - mSDF.at(*itr);
                    if (*itr + mResolutionZ < gridMaxNum)
                    {
                        //norX[1] = mSDF.at(*itr) - mSDF.at(*itr + mResolutionZ);
                        norX[1] = mSDF.at(*itr + mResolutionZ) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norX[1] = mSDF.at(*itr - mResolutionZ) - mSDF.at(*itr);
                        norX[1] = mSDF.at(*itr) - mSDF.at(*itr - mResolutionZ);
                    }
                    if (*itr + 1 < gridMaxNum)
                    {
                        //norX[2] = mSDF.at(*itr) - mSDF.at(*itr + 1);
                        norX[2] = mSDF.at(*itr + 1) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norX[2] = mSDF.at(*itr - 1) - mSDF.at(*itr);
                        norX[2] = mSDF.at(*itr) - mSDF.at(*itr - 1);
                    }
                    float norLen = norX.Normalise();
                    if (norLen > 1.0e-15)
                    {
                        posList.push_back(posX);
                        norList.push_back(norX);
                    }
                    else
                    {
                        DebugLog << "Normal Zero: X Direction" << std::endl;
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
                    float wr = fabs(sdfOrigin / (sdfOrigin - sdfY));
                    MagicMath::Vector3 posY(mMinX + xIndex * deltaX,
                        mMinY + (yIndex + wr) * deltaY, mMinZ + zIndex * deltaZ);
                    MagicMath::Vector3 norY;
                    if (*itr + xGridNum < gridMaxNum)
                    {
                        //norY[0] = mSDF.at(*itr) - mSDF.at(*itr + xGridNum);
                        norY[0] = mSDF.at(*itr + xGridNum) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norY[0] = mSDF.at(*itr - xGridNum) - mSDF.at(*itr);
                        norY[0] = mSDF.at(*itr) - mSDF.at(*itr - xGridNum);
                    }
                    //norY[1] = mSDF.at(*itr) - mSDF.at(*itr + mResolutionZ);
                    norY[1] = mSDF.at(*itr + mResolutionZ) - mSDF.at(*itr);
                    if (*itr + 1 < gridMaxNum)
                    {
                        //norY[2] = mSDF.at(*itr) - mSDF.at(*itr + 1);
                        norY[2] = mSDF.at(*itr + 1) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norY[2] = mSDF.at(*itr - 1) - mSDF.at(*itr);
                        norY[2] = mSDF.at(*itr) - mSDF.at(*itr - 1);
                    }
                    float norLen = norY.Normalise();
                    if (norLen > 1.0e-15)
                    {
                        posList.push_back(posY);
                        norList.push_back(norY);
                    }
                    else
                    {
                        DebugLog << "Normal Zero: Y direction" << std::endl;
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
                    float wr = fabs(sdfOrigin / (sdfOrigin - sdfZ));
                    MagicMath::Vector3 posZ(mMinX + xIndex * deltaX,
                        mMinY + yIndex * deltaY, mMinZ + (zIndex + wr) * deltaZ);
                    MagicMath::Vector3 norZ;
                    if (*itr + xGridNum < gridMaxNum)
                    {
                        //norZ[0] = mSDF.at(*itr) - mSDF.at(*itr + xGridNum);
                        norZ[0] = mSDF.at(*itr + xGridNum) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norZ[0] = mSDF.at(*itr - xGridNum) - mSDF.at(*itr);
                        norZ[0] = mSDF.at(*itr) - mSDF.at(*itr - xGridNum);
                    }
                    if (*itr + mResolutionZ < gridMaxNum)
                    {
                        //norZ[1] = mSDF.at(*itr) - mSDF.at(*itr + mResolutionZ);
                        norZ[1] = mSDF.at(*itr + mResolutionZ) - mSDF.at(*itr);
                    }
                    else
                    {
                        //norZ[1] = mSDF.at(*itr - mResolutionZ) - mSDF.at(*itr);
                        norZ[1] = mSDF.at(*itr) - mSDF.at(*itr - mResolutionZ);
                    }
                    //norZ[2] = mSDF.at(*itr) - mSDF.at(*itr + 1);
                    norZ[2] = mSDF.at(*itr + 1) - mSDF.at(*itr);

                    float norLen = norZ.Normalise();
                    if (norLen > 1.0e-15)
                    {
                        posList.push_back(posZ);
                        norList.push_back(norZ);
                    }
                    else
                    {
                        DebugLog << "Normal Zero: Z Direction" << std::endl;
                    }
                }
            }
        }

        int posNum = posList.size();
        //DebugLog << "Export point size: " << posNum << std::endl;
        if (posNum > 0)
        {
            Point3DSet* pPC = new Point3DSet;
            for (int i = 0; i < posNum; i++)
            {
                Point3D* point = new Point3D(posList.at(i), norList.at(i));
                pPC->InsertPoint(point);
            }
            //MagicDGP::Parser::ExportPointSet("interPointset.obj", pPC);//
            return pPC;
        }
        else
        {
            return NULL;
        }
    }

    void SignedDistanceFunction::ResetSDF()
    {
        for (std::set<int>::iterator itr = mPCIndex.begin(); itr != mPCIndex.end(); itr++)
        {
            mSDF.at(*itr) = 0.f;
            mWeight.at(*itr) = 0.f;
        }
        mPCIndex.clear();
    }
}
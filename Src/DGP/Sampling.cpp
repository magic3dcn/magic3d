#include "Sampling.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "flann/flann.h"
#include "Eigen/Eigenvalues"
#include <map>
#include <vector>

namespace MagicDGP
{
    Sampling::Sampling()
    {
    }

    Sampling::~Sampling()
    {
    }

    Point3DSet* Sampling::WLOPSampling(const Point3DSet* pPS, int sampleNum)
    {
        MagicLog << "Begin Sampling::WLOPSampling" << std::endl;
        std::vector<Vector3> samplePosList;
        InitialSampling(pPS, sampleNum, samplePosList);
        sampleNum = samplePosList.size();
        MagicLog << "Finsh Initial Sampling: " << sampleNum << std::endl;
        WLOPIteration(pPS, samplePosList);
        MagicLog << "Finish WLOP Iteration" << std::endl;
        std::vector<Vector3> norList;
        LocalPCANormalEstimate(samplePosList, norList);
        MagicLog << "Finish Normal Estimate" << std::endl;
        Point3DSet* pNewPS = new Point3DSet;
        for (int i = 0; i < sampleNum; i++)
        {
            Point3D* pPoint = new Point3D(samplePosList.at(i), norList.at(i));
            pNewPS->InsertPoint(pPoint);
        }
        return pNewPS;
    }

    void Sampling::InitialSampling(const Point3DSet* pPS, int sampleNum, std::vector<Vector3>& samplePosList)
    {
        int psNum = pPS->GetPointNumber();
        int delta = psNum / sampleNum;
        if (delta < 1)
        {
            MagicLog << "Sample number less than point set number" << std::endl;
            delta = 1;
            sampleNum = psNum;
        }
        samplePosList.clear();
        samplePosList.reserve(sampleNum);
        for (int i = 0; i < psNum; i += delta)
        {
            samplePosList.push_back(pPS->GetPoint(i)->GetPosition());
        }
    }

    void Sampling::WLOPIteration(const Point3DSet* pPS, std::vector<Vector3> & samplePosList)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        MagicLog << "Begin Sampling::WLOPIteration" << std::endl;
        int iterNum = 5;
        int iNum = samplePosList.size();
        int jNum = pPS->GetPointNumber();
        Vector3 bboxMin, bboxMax;
        pPS->GetBBox(bboxMin, bboxMax);
        Real smallValue = 1.0e-10;
        Real mu = 0.45;
        Real thetaScale = -1000;
        Real supportSize = 0.1f;
        Vector3 deltaBBox = bboxMax - bboxMin;
        int resolutionX = int(deltaBBox[0] / supportSize) + 1;
        int resolutionY = int(deltaBBox[1] / supportSize) + 1;
        int resolutionZ = int(deltaBBox[2] / supportSize) + 1;
        int resolutionYZ = resolutionY * resolutionZ;
        std::map<int, std::vector<int> > pcIndexMap;

        //construct pcIndexMap
        for (int pcIndex = 0; pcIndex < jNum; pcIndex++)
        {
            Vector3 deltaPos = pPS->GetPoint(pcIndex)->GetPosition() - bboxMin;
            int index = int(deltaPos[0] / supportSize) * resolutionYZ + int(deltaPos[1] / supportSize) * resolutionZ + int(deltaPos[2] / supportSize);
            pcIndexMap[index].push_back(pcIndex);
        }
        MagicLog << "Iteration prepare time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
        for (int kk = 0; kk < iterNum; kk++)
        {
            float iterateTimeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
            //construct sampleIndexMap
            std::map<int, std::vector<int> > sampleIndexMap;
            for (int sampleIndex = 0; sampleIndex < iNum; sampleIndex++)
            {
                Vector3 deltaPos = samplePosList.at(sampleIndex) - bboxMin;
                int index = int(deltaPos[0] / supportSize) * resolutionYZ + int(deltaPos[1] / supportSize) * resolutionZ + int(deltaPos[2] / supportSize);
                sampleIndexMap[index].push_back(sampleIndex);
            }
            
            std::vector<Vector3> samplePosBak = samplePosList;
            for (int i = 0; i < iNum; i++)
            {
                std::vector<Real> alpha(jNum, -1.f);
                std::vector<Real> beta(iNum, 1.f);
                Vector3 samplePosI = samplePosBak.at(i); 
                Vector3 sampleDeltaPos = samplePosI - bboxMin;
                int xIndex = sampleDeltaPos[0] / supportSize;
                int yIndex = sampleDeltaPos[1] / supportSize;
                int zIndex = sampleDeltaPos[2] / supportSize;
                Vector3 posRes1(0, 0, 0);
                Real alphaSum = 0;
                Vector3 posRes2(0, 0, 0);
                Real betaSum = 0;
                for (int xx = -1; xx <= 1; xx++)
                {
                    for (int yy = -1; yy <= 1; yy++)
                    {
                        for (int zz = -1; zz <= 1; zz++)
                        {
                            int blockIndex = (xIndex + xx) * resolutionYZ + (yIndex + yy) * resolutionZ + zIndex + zz;

                            int blockSize = pcIndexMap[blockIndex].size();
                            for (int mapIndex = 0; mapIndex < blockSize; mapIndex++)
                            {
                                int pointIndex = pcIndexMap[blockIndex].at(mapIndex);
                                if (alpha.at(pointIndex) < 0)
                                {
                                    Vector3 deltaPos = samplePosI - pPS->GetPoint(pointIndex)->GetPosition();
                                    Real deltaLen = deltaPos.Length();
                                    if (deltaLen < smallValue)
                                    {
                                        deltaLen = smallValue;
                                    }
                                    alpha.at(pointIndex) = exp(thetaScale * deltaLen * deltaLen) / deltaLen;
                                    //alpha.at(pointIndex) = 1.f;
                                }
                                posRes1 += pPS->GetPoint(pointIndex)->GetPosition() * alpha.at(pointIndex);
                                alphaSum += alpha.at(pointIndex);
                            }

                            blockSize = sampleIndexMap[blockIndex].size();
                            for (int mapIndex = 0; mapIndex < blockSize; mapIndex++)
                            {
                                int pointIndex = sampleIndexMap[blockIndex].at(mapIndex);
                                if (pointIndex == i)
                                {
                                    continue;
                                }
                                if (beta.at(pointIndex) > 0)
                                {
                                    Vector3 deltaPos = samplePosI - samplePosBak.at(pointIndex);
                                    Real deltaLen = deltaPos.Length();
                                    if (deltaLen < smallValue)
                                    {
                                        deltaLen = smallValue;
                                    }
                                    beta.at(pointIndex) = -exp(thetaScale * deltaLen * deltaLen) / deltaLen;
                                    //beta.at(pointIndex) = -1.f;
                                }
                                posRes2 += (samplePosI - samplePosBak.at(pointIndex)) * beta.at(pointIndex);
                                betaSum += beta.at(pointIndex);
                            }
                        }
                    }
                }//end for xx
                if (alphaSum < smallValue)
                {
                    alphaSum = smallValue;
                }
                posRes1 /= alphaSum;
                if (betaSum > -smallValue)
                {
                    betaSum = -smallValue;
                }
                posRes2 /= betaSum;
                samplePosList.at(i) = posRes1 + posRes2 * mu;

            } //end for i

            MagicLog << "WLOPIteration: " << kk << " time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - iterateTimeStart << std::endl;
        }//end for k

        MagicLog << "Iteration total time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
    }

    void Sampling::LocalPCANormalEstimate(const std::vector<Vector3>& samplePosList, std::vector<Vector3>& norList)
    {
        float startTime = MagicCore::ToolKit::GetSingleton()->GetTime();

        int pointNum = samplePosList.size();
        norList.clear();
        norList.reserve(pointNum);

        int dim = 3;
        int refNum = pointNum;
        float* dataSet = new float[refNum * dim];
        int searchNum = pointNum;
        float* searchSet = new float[searchNum * dim];
        for (int i = 0; i < pointNum; i++)
        {
            MagicDGP::Vector3 pos = samplePosList.at(i);
            dataSet[dim * i + 0] = pos[0];
            dataSet[dim * i + 1] = pos[1];
            dataSet[dim * i + 2] = pos[2];
            searchSet[dim * i + 0] = pos[0];
            searchSet[dim * i + 1] = pos[1];
            searchSet[dim * i + 2] = pos[2];
        }
        int nn = 20;
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

        MagicLog << "Sampling::LocalPCANormalEstimate, prepare time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - startTime << std::endl;

        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> es;
        for (int i = 0; i < samplePosList.size(); i++)
        {
            Vector3 pos = samplePosList.at(i);
            Vector3 deltaPos[20]; //nn
            int baseIndex = i * nn;
            for (int j = 0; j < nn; j++)
            {
                deltaPos[j] = samplePosList.at(pIndex[baseIndex + j]) - pos;
            }
            Eigen::Matrix3d mat;
            for (int xx = 0; xx < 3; xx++)
            {
                for (int yy = 0; yy < 3; yy++)
                {
                    Real v = 0;
                    for (int kk = 0; kk < nn; kk++)
                    {
                        v += deltaPos[kk][xx] * deltaPos[kk][yy];
                    }
                    mat(xx, yy) = v;
                }
            }//end for xx
            es.compute(mat);
            Eigen::Vector3d norvec = es.eigenvectors().col(0);
            Vector3 nor(norvec(0), norvec(1), norvec(2));
            Real norLen = nor.Normalise();
            if (norLen < Epsilon)
            {
                MagicLog << "Error: small normal length" << std::endl;
            }
            if (nor[2] < 0)
            {
                nor *= -1;
            }
            norList.push_back(nor);
        }
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
        MagicLog << "Sampling::LocalPCANormalEstimate, total time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - startTime << std::endl;
    }
}
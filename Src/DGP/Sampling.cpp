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

    Point3DSet* Sampling::PointSetWLOPSampling(const Point3DSet* pPS, int sampleNum)
    {
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Begin Sampling::PointSetWLOPSampling" << std::endl;
        std::vector<Vector3> samplePosList;
        InitialSampling(pPS, sampleNum, samplePosList);
        sampleNum = samplePosList.size();
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Finsh Initial Sampling: " << sampleNum << std::endl;
        WLOPIteration(pPS, samplePosList);
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Finish WLOP Iteration" << std::endl;
        std::vector<Vector3> norList;
        LocalPCANormalEstimate(samplePosList, norList);
        NormalConsistent(pPS, samplePosList, norList);
        sampleNum = samplePosList.size();
        //NormalSmooth(samplePosList, norList);
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Finish Normal Estimate" << std::endl;
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
            MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Sample number less than point set number" << std::endl;
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
        float timeStart = MagicCore::ToolKit::GetTime();
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Begin Sampling::WLOPIteration" << std::endl;
        int iNum = samplePosList.size();
        int jNum = pPS->GetPointNumber();
        int iterNum = 20;
        if (jNum > 1000000)
        {
            iterNum = 10;
        }
        else if (jNum > 10000)
        {
            iterNum = iterNum - jNum / 100000;
        }
        Vector3 bboxMin, bboxMax;
        pPS->GetBBox(bboxMin, bboxMax);
        Real smallValue = 1.0e-10;
        Real mu = 0.45;
        //Real thetaScale = 1000;
        //Real supportSize = 0.03f;
        Real supportSize = pPS->GetDensity() * 500;
        Real thetaScale = 100 / supportSize / supportSize;
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Density: " << pPS->GetDensity() << " supportSize: " << supportSize << " thetaScale: " << thetaScale << std::endl; 
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
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Iteration prepare time: " << MagicCore::ToolKit::GetTime() - timeStart << std::endl;
        for (int kk = 0; kk < iterNum; kk++)
        {
            float iterateTimeStart = MagicCore::ToolKit::GetTime();
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
                                Vector3 psPos = pPS->GetPoint(pointIndex)->GetPosition();
                                Vector3 deltaPos = samplePosI - psPos;
                                Real deltaLen = deltaPos.Length();
                                if (deltaLen < smallValue)
                                {
                                    deltaLen = smallValue;
                                }
                                //Real alpha = exp(-thetaScale * deltaLen * deltaLen) / deltaLen;
                                Real rTemp = thetaScale * deltaLen * deltaLen;
                                Real alpha = 1 / deltaLen / (1 + rTemp + rTemp * rTemp);
                                posRes1 += psPos * alpha;
                                alphaSum += alpha;
                            }

                            blockSize = sampleIndexMap[blockIndex].size();
                            for (int mapIndex = 0; mapIndex < blockSize; mapIndex++)
                            {
                                int pointIndex = sampleIndexMap[blockIndex].at(mapIndex);
                                if (pointIndex == i)
                                {
                                    continue;
                                }
                                Vector3 psPos = samplePosBak.at(pointIndex);
                                Vector3 deltaPos = samplePosI - psPos;
                                Real deltaLen = deltaPos.Length();
                                if (deltaLen < smallValue)
                                {
                                    deltaLen = smallValue;
                                }
                                //Real beta = -exp(-thetaScale * deltaLen * deltaLen) / deltaLen;
                                Real rTemp = thetaScale * deltaLen * deltaLen;
                                Real beta = -1 / deltaLen / (1 + rTemp + rTemp * rTemp);
                                posRes2 += (samplePosI - psPos) * beta;
                                betaSum += beta;
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

            MagicLog(MagicCore::LOGLEVEL_DEBUG) << "WLOPIteration: " << kk << " time: " << MagicCore::ToolKit::GetTime() - iterateTimeStart << std::endl;
        }//end for k

        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Iteration total time: " << MagicCore::ToolKit::GetTime() - timeStart << std::endl;
    }

    void Sampling::LocalPCANormalEstimate(const std::vector<Vector3>& samplePosList, std::vector<Vector3>& norList)
    {
        float startTime = MagicCore::ToolKit::GetTime();

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

        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Sampling::LocalPCANormalEstimate, prepare time: " << MagicCore::ToolKit::GetTime() - startTime << std::endl;

        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> es;
        for (int i = 0; i < pointNum; i++)
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
                MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Error: small normal length" << std::endl;
            }
            norList.push_back(nor);
        }
        //std::set<int> spreadSet;
        //spreadSet.insert(0);
        //std::vector<int> visitFlag(pointNum, 0);
        //visitFlag.at(0) = 1;
        //while (spreadSet.size() > 0)
        //{
        //    std::set<int> spreadSetNext;
        //    for (std::set<int>::iterator itr = spreadSet.begin(); itr != spreadSet.end(); itr++)
        //    {
        //        Vector3 nor = norList.at(*itr);
        //        int baseIndex = *itr * nn;
        //        //for (int i = 0; i < nn; i++)
        //        for (int i = 0; i < 9; i++)
        //        {
        //            int index = pIndex[baseIndex + i];
        //            if (visitFlag.at(index) == 1)
        //            {
        //                continue;
        //            }
        //            if (nor * norList.at(index) < 0)
        //            {
        //                norList.at(index) *= -1;
        //            }
        //            spreadSetNext.insert(index);
        //            visitFlag.at(index) = 1;
        //        }
        //    }// end for spreadSet
        //    spreadSet = spreadSetNext;
        //}
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
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Sampling::LocalPCANormalEstimate, total time: " << MagicCore::ToolKit::GetTime() - startTime << std::endl;
    }

    void Sampling::NormalConsistent(const Point3DSet* pPS, std::vector<Vector3>& samplePosList, std::vector<Vector3>& norList)
    {
        float startTime = MagicCore::ToolKit::GetTime();

        int dim = 3;
        int refNum = pPS->GetPointNumber();
        float* dataSet = new float[refNum * dim];
        for (int i = 0; i < refNum; i++)
        {
            Vector3 pos = pPS->GetPoint(i)->GetPosition();
            dataSet[dim * i + 0] = pos[0];
            dataSet[dim * i + 1] = pos[1];
            dataSet[dim * i + 2] = pos[2];
        }
        int searchNum = samplePosList.size();
        float* searchSet = new float[searchNum * dim];
        for (int i = 0; i < searchNum; i++)
        {
            Vector3 pos = samplePosList.at(i);
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

        std::vector<Vector3> newPosList, newNorList;
        for (int i = 0; i < searchNum; i++)
        {
            Vector3 norRef(0, 0, 0);
            int baseIndex = i * nn;
            float norError = 0;
            for (int j = 0; j < nn; j++)
            {
                norRef += pPS->GetPoint(pIndex[baseIndex + j])->GetNormal();
                norError += fabs(pPS->GetPoint(pIndex[baseIndex + j])->GetNormal() * norList.at(i));
            }
            norRef.Normalise();
            norError /= nn;
            if (norError > 0.707)
            {
                newPosList.push_back(samplePosList.at(i));
                if (norList.at(i) * norRef < 0)
                {
                    norList.at(i) *= -1;
                }
                newNorList.push_back(norList.at(i));
            }
        }
        samplePosList = newPosList;
        norList = newNorList;

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
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Sampling::NormalConsistent, total time: " << MagicCore::ToolKit::GetTime() - startTime << std::endl;
    }

    void Sampling::NormalSmooth(std::vector<Vector3>& samplePosList, std::vector<Vector3>& norList)
    {
        float startTime = MagicCore::ToolKit::GetTime();

        int pointNum = samplePosList.size();
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
        int nn = pointNum / 100;
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

        int smoothNum = nn / 2;
        for (int k = 0; k < smoothNum; k++)
        {
            std::vector<Vector3> newNorList(pointNum);
            for (int i = 0; i < pointNum; i++)
            {
                int avgFlag = 0;
                Vector3 nor = norList.at(i);
                int baseIndex = nn * i;
                for (int j = 0; j < nn; j++)
                {
                    Vector3 norNeighbor = norList.at(pIndex[baseIndex + j]);
                    if (norNeighbor * nor > 0)
                    {
                        avgFlag++;
                    }
                    else
                    {
                        avgFlag--;
                    }
                }
                if (avgFlag < 0)
                {
                    newNorList.at(i) = norList.at(i) * (-1.f);
                }
                else
                {
                    newNorList.at(i) = norList.at(i);
                }
            }
            norList = newNorList;
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
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Sampling::NormalSmooth: " << MagicCore::ToolKit::GetTime() - startTime << std::endl;
    }

    Point3DSet* Sampling::PointSetUniformSampling(Point3DSet* pPS, int sampleNum)
    {
        float timeStart = MagicCore::ToolKit::GetTime();
        int psNum = pPS->GetPointNumber();
        if (sampleNum > psNum)
        {
            DebugLog << "Error: sampleNum > pointNum" << std::endl;
            return NULL;
        }
        std::vector<bool> sampleFlag(psNum, 0);
        std::vector<int> sampleIndex(sampleNum);
        sampleFlag.at(0) = true;
        sampleIndex.at(0) = 0;
        std::vector<Real> minDist(psNum, 1.0e10);
        int curIndex = 0;
        for (int sid = 1; sid < sampleNum; ++sid)
        {
            Vector3 curPos = pPS->GetPoint(curIndex)->GetPosition();
            Real maxDist = -1;
            int pos = -1;
            for (int vid = 0; vid < psNum; ++vid)
            {
                if (sampleFlag.at(vid) == 1)
                {
                    continue;
                }
                Real dist = (pPS->GetPoint(vid)->GetPosition() - curPos).LengthSquared();
                if (dist < minDist.at(vid))
                {
                    minDist.at(vid) = dist;
                }
                if (minDist.at(vid) > maxDist)
                {
                    maxDist = minDist.at(vid);
                    pos = vid;
                }
            }
            sampleIndex.at(sid) = pos;
            curIndex = pos;
            sampleFlag.at(pos) = 1;
        }
        DebugLog << "Sampling time: " << MagicCore::ToolKit::GetTime() - timeStart << std::endl;
        MagicDGP::Point3DSet* pNewPS = new MagicDGP::Point3DSet;
        for (int sid = 0; sid < sampleNum; ++sid)
        {
            MagicDGP::Point3D* pPoint = pPS->GetPoint(sampleIndex.at(sid));
            MagicDGP::Point3D* pNewPoint = new MagicDGP::Point3D(pPoint->GetPosition(), pPoint->GetNormal());
            pNewPS->InsertPoint(pNewPoint);
        }

        return pNewPS;
    }

    int Sampling::MeshVertexUniformSampling(const Mesh3D* pMesh, int sampleNum, std::vector<int>& sampleIndex)
    {
        float timeStart = MagicCore::ToolKit::GetTime();
        int vertNum = pMesh->GetVertexNumber();
        if (sampleNum > vertNum)
        {
            sampleNum = vertNum;
        }
        std::vector<bool> sampleFlag(vertNum, 0);
        sampleFlag.at(0) = true;
        sampleIndex = std::vector<int>(sampleNum);
        sampleIndex.at(0) = 0;
        std::vector<Real> minDist(vertNum, 1.0e10);
        int curIndex = 0;
        for (int sid = 1; sid < sampleNum; ++sid)
        {
            Vector3 curPos = pMesh->GetVertex(curIndex)->GetPosition();
            Real maxDist = -1;
            int pos = -1;
            for (int vid = 0; vid < vertNum; ++vid)
            {
                if (sampleFlag.at(vid) == 1)
                {
                    continue;
                }
                Real dist = (pMesh->GetVertex(vid)->GetPosition() - curPos).LengthSquared();
                if (dist < minDist.at(vid))
                {
                    minDist.at(vid) = dist;
                }
                if (minDist.at(vid) > maxDist)
                {
                    maxDist = minDist.at(vid);
                    pos = vid;
                }
            }
            sampleIndex.at(sid) = pos;
            curIndex = pos;
            sampleFlag.at(pos) = 1;
        }
        DebugLog << "Sampling time: " << MagicCore::ToolKit::GetTime() - timeStart << std::endl;
        return sampleNum;
    }

    bool Sampling::SimplifyMesh(Mesh3D* pMesh, int targetNum)
    {
        int vertNum = pMesh->GetVertexNumber();
        if (targetNum >= vertNum)
        {
            InfoLog << "Consolidation::SimplifyMesh target vertex number is less than vertex number." << std::endl;
            return false;
        }
        std::vector<HomoMatrix4> quadricMatList;
        CalQuadricMatrix(pMesh, quadricMatList);
        int collapseNum = vertNum - targetNum;
        for (int collapseId = 0; collapseId < collapseNum; collapseId++)
        {
            int colEdgeId = ChooseCollapseEdge(pMesh, quadricMatList);
            CollapseEdge(pMesh, colEdgeId, quadricMatList);
        }
        //Update mesh structure

    }

    void Sampling::CalQuadricMatrix(Mesh3D* pMesh, std::vector<HomoMatrix4>& quadricMatList)
    {
        int vertNum = pMesh->GetVertexNumber();
        int faceNum = pMesh->GetFaceNumber();
        quadricMatList.clear();
        quadricMatList = std::vector<HomoMatrix4>(vertNum);
        std::vector<std::vector<Real> > faceQuadricMat = std::vector<std::vector<Real> >(faceNum);
        for (int fid = 0; fid < faceNum; fid++)
        {
            Edge3D* pEdge = pMesh->GetFace(fid)->GetEdge();
            Vector3 pos0 = pEdge->GetVertex()->GetPosition();
            Vector3 pos1 = pEdge->GetNext()->GetVertex()->GetPosition();
            Vector3 pos2 = pEdge->GetPre()->GetVertex()->GetPosition();
            Vector3 nor = (pos1 - pos0).CrossProduct(pos2 - pos0);
            nor.Normalise();
            Real d = nor * pos0 * (-1.0);

        }
    }

    int Sampling::ChooseCollapseEdge(Mesh3D* pMesh, std::vector<HomoMatrix4>& quadricMatrix)
    {
        return 0;
    }

    void Sampling::CollapseEdge(Mesh3D* pMesh, int edgeId, std::vector<HomoMatrix4>& quadricMatList)
    {

    }
}
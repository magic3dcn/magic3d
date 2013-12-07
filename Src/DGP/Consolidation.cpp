#include "Consolidation.h"
#include "flann/flann.h"
#include "Eigen/Eigenvalues"

namespace MagicDGP
{
    Consolidation::Consolidation()
    {
    }

    Consolidation::~Consolidation()
    {
    }

    void Consolidation::CalPointSetNormal(Point3DSet* pPointSet)
    {
        int pointNum = pPointSet->GetPointNumber();
        std::vector<Vector3> norList(pointNum);

        int dim = 3;
        int refNum = pointNum;
        float* dataSet = new float[refNum * dim];
        int searchNum = pointNum;
        float* searchSet = new float[searchNum * dim];
        for (int pid = 0; pid < pointNum; pid++)
        {
            MagicDGP::Vector3 pos = pPointSet->GetPoint(pid)->GetPosition();
            dataSet[dim * pid + 0] = pos[0];
            dataSet[dim * pid + 1] = pos[1];
            dataSet[dim * pid + 2] = pos[2];
            searchSet[dim * pid + 0] = pos[0];
            searchSet[dim * pid + 1] = pos[1];
            searchSet[dim * pid + 2] = pos[2];
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

        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> es;
        for (int pid = 0; pid < pointNum; pid++)
        {
            Vector3 pos = pPointSet->GetPoint(pid)->GetPosition();
            Vector3 deltaPos[20]; //nn
            int baseIndex = pid * nn;
            for (int j = 0; j < nn; j++)
            {
                deltaPos[j] = pPointSet->GetPoint(pIndex[baseIndex + j])->GetPosition() - pos;
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
                DebugLog << "Error: small normal length" << std::endl;
            }
            norList.at(pid) = nor;
        }
        if (pDist != NULL)
        {
            delete []pDist;
            pDist = NULL;
        }
        //Make normal consitent
        std::multimap<Real, int> prioritySet;
        std::vector<bool> acceptMark(pointNum, 0);
        while (true)
        {
            //find activeId
            int activeId = -1;
            if (prioritySet.size() > 0)
            {
                std::multimap<Real, int>::iterator firstItr = prioritySet.begin();
                activeId = firstItr->second;
                prioritySet.erase(firstItr);
            }
            else 
            {
                Real maxZ = -1.0e10;
                int maxZId = -1;
                for (int pid = 0; pid < pointNum; pid++)
                {
                    if (acceptMark.at(pid) == 0)
                    {
                        Real z = pPointSet->GetPoint(pid)->GetPosition()[2];
                        if (z > maxZ)
                        {
                            maxZ = z;
                            maxZId = pid;
                        }
                    }
                }
                if (maxZId > -1)
                {
                    if (norList.at(maxZId)[2] < 0)
                    {
                        norList.at(maxZId) *= -1;
                    }
                }
                activeId = maxZId;
                //DebugLog << "new start point" << std::endl;
            }
            if (activeId == -1)
            {
                break;
            }

            acceptMark.at(activeId) = 1;
            //add new point to priority set
            int baseIndex = activeId * nn; 
            for (int nid = 0; nid < nn; nid++)
            {
                int nIndex = pIndex[baseIndex + nid];
                if (acceptMark.at(nIndex) == true)
                {
                    continue;
                }
                else
                {
                    Real cosAngle = norList.at(activeId) * norList.at(nIndex);
                    if (cosAngle < 0)
                    {
                        norList.at(nIndex) *= -1;
                    }
                    prioritySet.insert( std::pair<Real, int>(1.0 - fabs(cosAngle), nIndex) );
                    acceptMark.at(nIndex) = true;
                }
            }
        }

        for (int pid = 0; pid < pointNum; pid++)
        {
            pPointSet->GetPoint(pid)->SetNormal(norList.at(pid));
        }
        //
        if (pIndex != NULL)
        {
            delete []pIndex;
            pIndex = NULL;
        }
        pPointSet->SetHasNormal(true);
    }

    bool Consolidation::RedressPointSetNormal(Point3DSet* pPointSet)
    {
        if (pPointSet->HasNormal() == false)
        {
            WarnLog << "Consolidation::RedressPointSetNormal need point set normal, but this one has no normal" << std::endl;
            return false;
        }

        int pointNum = pPointSet->GetPointNumber();
        int dim = 3;
        int refNum = pointNum;
        float* dataSet = new float[refNum * dim];
        int searchNum = pointNum;
        float* searchSet = new float[searchNum * dim];
        for (int pid = 0; pid < pointNum; pid++)
        {
            MagicDGP::Vector3 pos = pPointSet->GetPoint(pid)->GetPosition();
            dataSet[dim * pid + 0] = pos[0];
            dataSet[dim * pid + 1] = pos[1];
            dataSet[dim * pid + 2] = pos[2];
            searchSet[dim * pid + 0] = pos[0];
            searchSet[dim * pid + 1] = pos[1];
            searchSet[dim * pid + 2] = pos[2];
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

        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> es;
        for (int pid = 0; pid < pointNum; pid++)
        {
            Point3D* pPoint = pPointSet->GetPoint(pid);
            Vector3 pos = pPoint->GetPosition();
            Vector3 deltaPos[20]; //nn
            int baseIndex = pid * nn;
            for (int j = 0; j < nn; j++)
            {
                deltaPos[j] = pPointSet->GetPoint(pIndex[baseIndex + j])->GetPosition() - pos;
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
                DebugLog << "Error: small normal length" << std::endl;
            }
            if (nor * (pPoint->GetNormal()) < 0)
            {
                nor *= (-1);
            }
            pPoint->SetNormal(nor);
        }
        if (pDist != NULL)
        {
            delete []pDist;
            pDist = NULL;
        }
        if (pIndex != NULL)
        {
            delete []pIndex;
            pIndex = NULL;
        }
    }
}
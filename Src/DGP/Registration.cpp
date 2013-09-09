#include "StdAfx.h"
#include "Registration.h"
#include "flann/flann.h"
#include "Eigen/Dense"

namespace MagicDGP
{
    Registration::Registration()
    {
    }

    Registration::~Registration()
    {
    }

    void Registration::ICPRegistrate(const Point3DSet* pRef, Point3DSet* pOrigin)
    {
        std::vector<int> sampleIndex;
        ICPSamplePoint(pOrigin, sampleIndex);
        std::vector<int> correspondIndex;
        ICPFindCorrespondance(pRef, pOrigin, sampleIndex, correspondIndex);
        ICPEnergyMinimization(pRef, pOrigin, sampleIndex, correspondIndex);
    }

    void Registration::ICPRegistrate(const Point3DSet* pRef, Point3DSet* pOrigin, const HomoMatrix4* pTransInit, HomoMatrix4* pTransRes)
    {
        int iterNum = 5;
        *pTransRes = *pTransInit;
        for (int k = 0; k < iterNum; k++)
        {
            std::vector<int> sampleIndex;
            ICPSamplePoint(pOrigin, sampleIndex);
            std::vector<int> correspondIndex;
            ICPFindCorrespondance(pRef, pOrigin, pTransRes, sampleIndex, correspondIndex);
            HomoMatrix4 transDelta;
            ICPEnergyMinimization(pRef, pOrigin, pTransRes, sampleIndex, correspondIndex, &transDelta);
            //*pTransRes *= transDelta;
            *pTransRes = transDelta * (*pTransRes);
        }
    }

    void Registration::ICPSamplePoint(const Point3DSet* pPC, std::vector<int>& sampleIndex)
    {
        MagicLog << "egistration::ICPSamplePoint" << std::endl;
        int pcNum = pPC->GetPointNumber();
        for (int i = 0; i < pcNum; i += 1)
        {
            sampleIndex.push_back(i);
        }
    }

    void Registration::ICPFindCorrespondance(const Point3DSet* pRef, const Point3DSet* pOrigin, std::vector<int>& sampleIndex,  std::vector<int>& correspondIndex)
    {
        MagicLog << "Registration::ICPFindCorrespondance" << std::endl;
        int dim = 3;
        int refNum = pRef->GetPointNumber();
        float* dataSet = new float[refNum * dim];
        for (int i = 0; i < refNum; i++)
        {
            Vector3 pos = pRef->GetPoint(i)->GetPosition();
            dataSet[dim * i + 0] = pos[0];
            dataSet[dim * i + 1] = pos[1];
            dataSet[dim * i + 2] = pos[2];
        }
        int searchNum = sampleIndex.size();
        float* searchSet = new float[searchNum * dim];
        for (int i = 0; i < searchNum; i++)
        {
            Vector3 pos = pOrigin->GetPoint(sampleIndex.at(i))->GetPosition();
            searchSet[dim * i + 0] = pos[0];
            searchSet[dim * i + 1] = pos[1];
            searchSet[dim * i + 2] = pos[2];
        }
        int nn = 1;
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

        //delete wrong correspondance
        float distThre = 100.f;
        float norThre = 0.906307787f; //cos(25);
        std::vector<int> sampleIndexBak = sampleIndex;
        sampleIndex.clear();
        correspondIndex.clear();
        for (int i = 0; i < searchNum; i++)
        {
            if (pDist[i] > distThre)
            {
        //        MagicLog << "Large dist: " << pDist[i] << std::endl;
                continue;
            }
            float norDist = pRef->GetPoint(pIndex[i])->GetNormal() * (pOrigin->GetPoint(sampleIndexBak.at(i))->GetNormal());
            if (norDist < norThre || norDist > 1.0)
            {
         //       MagicLog << "Large Nor: " << norDist << std::endl;
                continue;
            }
            sampleIndex.push_back(sampleIndexBak.at(i));
            correspondIndex.push_back(pIndex[i]);
        }
        MagicLog << "Sample Number: " << sampleIndex.size() << std::endl;
        delete []pIndex;
        delete []pDist;
    }

    void Registration::ICPFindCorrespondance(const Point3DSet* pRef, const Point3DSet* pOrigin, const HomoMatrix4* pTransInit,
            std::vector<int>& sampleIndex,  std::vector<int>& correspondIndex)
    {
        MagicLog << "Registration::ICPFindCorrespondance" << std::endl;
        int dim = 3;
        int refNum = pRef->GetPointNumber();
        float* dataSet = new float[refNum * dim];
        for (int i = 0; i < refNum; i++)
        {
            Vector3 pos = pRef->GetPoint(i)->GetPosition();
            dataSet[dim * i + 0] = pos[0];
            dataSet[dim * i + 1] = pos[1];
            dataSet[dim * i + 2] = pos[2];
        }
        int searchNum = sampleIndex.size();
        float* searchSet = new float[searchNum * dim];
        for (int i = 0; i < searchNum; i++)
        {
            Vector3 pos = pTransInit->TransformPoint( pOrigin->GetPoint(sampleIndex.at(i))->GetPosition() );
            searchSet[dim * i + 0] = pos[0];
            searchSet[dim * i + 1] = pos[1];
            searchSet[dim * i + 2] = pos[2];
        }
        int nn = 1;
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

        //delete wrong correspondance
        float distThre = 100.f;
        float norThre = 0.98f; //cos(11);
        std::vector<int> sampleIndexBak = sampleIndex;
        sampleIndex.clear();
        correspondIndex.clear();
        for (int i = 0; i < searchNum; i++)
        {
            if (pDist[i] > distThre)
            {
        //        MagicLog << "Large dist: " << pDist[i] << std::endl;
                continue;
            }
            float norDist = pRef->GetPoint(pIndex[i])->GetNormal() * (pTransInit->RotateVector( pOrigin->GetPoint(sampleIndexBak.at(i))->GetNormal() ));
            if (norDist < norThre || norDist > 1.0)
            {
         //       MagicLog << "Large Nor: " << norDist << std::endl;
                continue;
            }
            sampleIndex.push_back(sampleIndexBak.at(i));
            correspondIndex.push_back(pIndex[i]);
        }
        MagicLog << "Sample Number: " << sampleIndex.size() << std::endl;
        delete []pIndex;
        delete []pDist;
    }

    void Registration::ICPEnergyMinimization(const Point3DSet* pRef, Point3DSet* pOrigin, std::vector<int>& sampleIndex, std::vector<int>& correspondIndex)
    {
        MagicLog << "Registration::ICPEnergyMinimization" << std::endl;
        int pcNum = sampleIndex.size();
        Eigen::MatrixXd matA(pcNum, 6);
        Eigen::VectorXd vecB(pcNum, 1);
        for (int i = 0; i < pcNum; i++)
        {
            Vector3 norRef = pRef->GetPoint(correspondIndex.at(i))->GetNormal();
            Vector3 posRef = pRef->GetPoint(correspondIndex.at(i))->GetPosition();
            Vector3 posPC  = pOrigin->GetPoint(sampleIndex.at(i))->GetPosition();
            vecB(i) = (posRef - posPC) * norRef;
            Vector3 coffTemp = posPC.CrossProduct(norRef);
            matA(i, 0) = coffTemp[0];
            matA(i, 1) = coffTemp[1];
            matA(i, 2) = coffTemp[2];
            matA(i, 3) = norRef[0];
            matA(i, 4) = norRef[1];
            matA(i, 5) = norRef[2];
        }
        Eigen::MatrixXd matAT = matA.transpose();
        Eigen::MatrixXd matCoefA = matAT * matA;
        Eigen::MatrixXd vecCoefB = matAT * vecB;
        Eigen::VectorXd res = matCoefA.ldlt().solve(vecCoefB);
        //print result
        MagicLog << "MatA: " << std::endl;
        MagicLog << 1 << " " << -res(2) << " " << res(1) << " " << res(3) << std::endl;
        MagicLog << res(2) << " " << 1 << " " << -res(0) << " " << res(4) << std::endl;
        MagicLog << -res(1) << " " << res(0) << " " << 1 << " " << res(5) << std::endl;
        //update pOrigin position
        for (int i = 0; i < pcNum; i++)
        {
            Vector3 pos = pOrigin->GetPoint(i)->GetPosition();
            Vector3 newPos;
            newPos[0] = pos[0] - pos[1] * res[2] + pos[2] * res[1] + res[3];
            newPos[1] = pos[0] * res[2] + pos[1] - pos[2] * res[0] + res[4];
            newPos[2] = -pos[0] * res[1] + pos[1] * res[0] + pos[2] + res[5];
            pOrigin->GetPoint(i)->SetPosition(newPos);
        }
    }

    void Registration::ICPEnergyMinimization(const Point3DSet* pRef, const Point3DSet* pOrigin, const HomoMatrix4* pTransInit, 
            std::vector<int>& sampleIndex, std::vector<int>& correspondIndex, HomoMatrix4* pTransDelta)
    {
        MagicLog << "Registration::ICPEnergyMinimization" << std::endl;
        int pcNum = sampleIndex.size();
        Eigen::MatrixXd matA(pcNum, 6);
        Eigen::VectorXd vecB(pcNum, 1);
        for (int i = 0; i < pcNum; i++)
        {
            Vector3 norRef = pRef->GetPoint(correspondIndex.at(i))->GetNormal();
            Vector3 posRef = pRef->GetPoint(correspondIndex.at(i))->GetPosition();
            Vector3 posPC  = pTransInit->TransformPoint( pOrigin->GetPoint(sampleIndex.at(i))->GetPosition() );
            vecB(i) = (posRef - posPC) * norRef;
            Vector3 coffTemp = posPC.CrossProduct(norRef);
            matA(i, 0) = coffTemp[0];
            matA(i, 1) = coffTemp[1];
            matA(i, 2) = coffTemp[2];
            matA(i, 3) = norRef[0];
            matA(i, 4) = norRef[1];
            matA(i, 5) = norRef[2];
        }
        Eigen::MatrixXd matAT = matA.transpose();
        Eigen::MatrixXd matCoefA = matAT * matA;
        Eigen::MatrixXd vecCoefB = matAT * vecB;
        Eigen::VectorXd res = matCoefA.ldlt().solve(vecCoefB);
        pTransDelta->Unit();
        pTransDelta->SetValue(0, 1, -res(2));
        pTransDelta->SetValue(0, 2, res(1));
        pTransDelta->SetValue(0, 3, res(3));
        pTransDelta->SetValue(1, 0, res(2));
        pTransDelta->SetValue(1, 2, -res(0));
        pTransDelta->SetValue(1, 3, res(4));
        pTransDelta->SetValue(2, 0, -res(1));
        pTransDelta->SetValue(2, 1, res(0));
        pTransDelta->SetValue(2, 3, res(5));
        //print result
        MagicLog << "MatA: " << std::endl;
        MagicLog << 1 << " " << -res(2) << " " << res(1) << " " << res(3) << std::endl;
        MagicLog << res(2) << " " << 1 << " " << -res(0) << " " << res(4) << std::endl;
        MagicLog << -res(1) << " " << res(0) << " " << 1 << " " << res(5) << std::endl;
    }
}

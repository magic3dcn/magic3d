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

    void Registration::ICPRegistrate(const Point3DSet* pRef, Point3DSet* pPC)
    {
        std::vector<int> sampleIndex;
        ICPSamplePoint(pPC, sampleIndex);
        std::vector<int> correspondIndex;
        ICPFindCorrespondance(pRef, pPC, sampleIndex, correspondIndex);
        ICPEnergyMinimization(pRef, pPC, sampleIndex, correspondIndex);
    }

    void Registration::ICPSamplePoint(const Point3DSet* pPC, std::vector<int>& sampleIndex)
    {
        int pcNum = pPC->GetPointNumber();
        for (int i = 0; i < pcNum; i += 300)
        {
            sampleIndex.push_back(i);
        }
    }

    void Registration::ICPFindCorrespondance(const Point3DSet* pRef, const Point3DSet* pPC, std::vector<int>& sampleIndex,  std::vector<int>& correspondIndex)
    {
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
            Vector3 pos = pPC->GetPoint(sampleIndex.at(i))->GetPosition();
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
        float distThre = 10.f;
        float norThre = 0.98f; //cos(11);
        std::vector<int> sampleIndexBak = sampleIndex;
        sampleIndex.clear();
        correspondIndex.clear();
        for (int i = 0; i < searchNum; i++)
        {
            if (pDist[i] > distThre)
            {
                continue;
            }
            float norDist = pRef->GetPoint(pIndex[i])->GetNormal() * (pPC->GetPoint(sampleIndexBak.at(i))->GetNormal());
            if (norDist > norThre)
            {
                continue;
            }
            sampleIndex.push_back(sampleIndexBak.at(i));
            correspondIndex.push_back(pIndex[i]);
        }

        delete []pIndex;
        delete []pDist;
    }

    void Registration::ICPEnergyMinimization(const Point3DSet* pRef, Point3DSet* pPC, std::vector<int>& sampleIndex, std::vector<int>& correspondIndex)
    {

    }
}

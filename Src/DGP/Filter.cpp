//#include "StdAfx.h"
#include "Filter.h"
#include "flann/flann.h"
#include "../Common/LogSystem.h"

namespace MagicDGP
{
    Filter::Filter()
    {
    }

    Filter::~Filter()
    {
    }

    void Filter::FilterDepthScanPointSet(Point3DSet* pPCSet)
    {
        MagicLog << "Filter::FilterDepthScanPointSet" << std::endl;
        int* pResIndex = NULL;
        float* pResDist = NULL;
        int nn = 24;
        FindNearestNeighbors(pPCSet, nn, &pResIndex, &pResDist);
        MagicLog << "Begin avg filter" << std::endl;
        int pcNum = pPCSet->GetPointNumber();
        std::vector<Vector3> posList(pcNum);
        for (int i = 0; i < pcNum; i++)
        {
            posList.at(i) = pPCSet->GetPoint(i)->GetPosition();
        }
        float threshold = 500.f;
        float w = 0.8f;
        for (int i = 0; i < pcNum; i++)
        {
            Vector3 neiPos(0.f, 0.f, 0.f);
            int neiNum = 0;
            for (int j = 0; j < nn; j++)
            {
                if (pResDist[i * nn + j] < threshold)
                {
                    neiPos += posList.at(pResIndex[i * nn + j]);
                    neiNum++;
                }
                else
                {
                    MagicLog << "Exceed threadhold: " << pResDist[i * nn + j] << std::endl;
                }
            }
            if (neiNum > 0)
            {
                Vector3 avgPos = posList.at(i) * w + neiPos * (1.f - w) / float(neiNum);
                pPCSet->GetPoint(i)->SetPosition(avgPos);
            }
        }
        //free memory
        if (pResIndex != NULL)
        {
            delete []pResIndex;
            pResIndex = NULL;
        }
        if (pResDist != NULL)
        {
            delete []pResDist;
            pResDist = NULL;
        }
    }

    void Filter::FindNearestNeighbors(Point3DSet* pPCSet, int nn, int** pIndex, float** pDist)
    {
        MagicLog << "Filter::FindNearestNeighbors" << std::endl;
        int pcNum = pPCSet->GetPointNumber();
        int dim = 3;
        float* dataSet = new float[pcNum * dim];
        float* searchSet = new float[pcNum * dim];
        for (int i = 0; i < pcNum; i++)
        {
            Vector3 pos = pPCSet->GetPoint(i)->GetPosition();
            dataSet[dim * i + 0] = pos[0];
            dataSet[dim * i + 1] = pos[1];
            dataSet[dim * i + 2] = pos[2];
            searchSet[dim * i + 0] = pos[0];
            searchSet[dim * i + 1] = pos[1];
            searchSet[dim * i + 2] = pos[2];
        }
        *pIndex = new int[pcNum * nn];
        *pDist = new float[pcNum * nn];
        FLANNParameters searchPara;
        searchPara = DEFAULT_FLANN_PARAMETERS;
        searchPara.algorithm = FLANN_INDEX_KDTREE;
        searchPara.trees = 8;
        searchPara.log_level = FLANN_LOG_INFO;
	    searchPara.checks = 64;
        float speedup;
        flann_index_t indexId = flann_build_index(dataSet, pcNum, dim, &speedup, &searchPara);
        flann_find_nearest_neighbors_index(indexId, searchSet, pcNum, *pIndex, *pDist, nn, &searchPara);

        flann_free_index(indexId, &searchPara);
        delete []dataSet;
        delete []searchSet;
    }
}
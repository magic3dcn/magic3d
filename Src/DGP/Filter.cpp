//#include "StdAfx.h"
#include "Filter.h"
#include "flann/flann.h"
#include "../Common/LogSystem.h"
#include <map>

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

    Mesh3D* Filter::RemoveSmallMeshPatch(Mesh3D* pMesh)
    {
        MagicLog << "Filter::RemoveSmallMeshPatch" << std::endl;
        int vertNum = pMesh->GetVertexNumber();
        std::vector<int> visitFlag(vertNum, 0);
        std::vector<std::vector<int> > vertGroups;
        while (1)
        {
            int startIndex = -1;
            for (int i = 0; i < vertNum; i++)
            {
                if (visitFlag.at(i) == 0)
                {
                    startIndex = i;
                    break;
                }
            }
            if (startIndex == -1)
            {
                break;
            }
            else
            {
                std::vector<int> vertOneGroup;
                std::vector<int> visitStack;
                visitStack.push_back(startIndex);
                vertOneGroup.push_back(startIndex);
                visitFlag.at(startIndex) = 1;
                while (visitStack.size() > 0)
                {
                    std::vector<int> visitStackNext;
                    for (std::vector<int>::iterator itr = visitStack.begin(); itr != visitStack.end(); ++itr)
                    {
                        Vertex3D* pVert = pMesh->GetVertex(*itr);
                        Edge3D* pEdge = pVert->GetEdge();
                        do
                        {
                            if (pEdge != NULL)
                            {
                                int newVertId = pEdge->GetVertex()->GetId();
                                if (visitFlag.at(newVertId) == 0)
                                {
                                    visitStackNext.push_back(newVertId);
                                    vertOneGroup.push_back(newVertId);
                                    visitFlag.at(newVertId) = 1;
                                }
                                pEdge = pEdge->GetPair()->GetNext();
                            }
                            else
                            {
                                break;
                            }
                        } while (pEdge != pVert->GetEdge());
                    }//end for visitStack
                    visitStack = visitStackNext;
                }//end While
                vertGroups.push_back(vertOneGroup);
            }
        }
        //Remove small patch
        int smallNum = vertNum / 100;
        smallNum = smallNum < 10 ? 10 : (smallNum > 500 ? 500 : smallNum);
        std::map<int, int> vertMapOld2New;
        Mesh3D* pNewMesh = new Mesh3D;
        int newMeshVertIndex = 0;
        std::vector<int> vertValidFlag(vertNum, 0);
        for (int i = 0; i < vertGroups.size(); i++)
        {
            int oneGroupNum = vertGroups.at(i).size();
            if (oneGroupNum > smallNum)
            {
                for (int j = 0; j < oneGroupNum; j++)
                {
                    int vertIndex = vertGroups.at(i).at(j);
                    pNewMesh->InsertVertex(pMesh->GetVertex(vertIndex)->GetPosition());
                    vertMapOld2New[vertIndex] = newMeshVertIndex;
                    newMeshVertIndex++;
                    vertValidFlag.at(vertIndex) = 1;
                }
            }
        }
        int faceNum = pMesh->GetFaceNumber();
        for (int i = 0; i < faceNum; i++)
        {
            Edge3D* pEdge = pMesh->GetFace(i)->GetEdge();
            int vertId0 = pEdge->GetVertex()->GetId();
            if (vertValidFlag.at(vertId0) == 0)
            {
                continue;
            }
            int vertId1 = pEdge->GetNext()->GetVertex()->GetId();
            if (vertValidFlag.at(vertId1) == 0)
            {
                continue;
            }
            int vertId2 = pEdge->GetPre()->GetVertex()->GetId();
            if (vertValidFlag.at(vertId2) == 0)
            {
                continue;
            }
            std::vector<Vertex3D* > newVertList;
            newVertList.push_back( pNewMesh->GetVertex(vertMapOld2New[vertId0]) );
            newVertList.push_back( pNewMesh->GetVertex(vertMapOld2New[vertId1]) );
            newVertList.push_back( pNewMesh->GetVertex(vertMapOld2New[vertId2]) );
            pNewMesh->InsertFace(newVertList);
        }
        pNewMesh->UpdateNormal();
        return pNewMesh;
    }
}
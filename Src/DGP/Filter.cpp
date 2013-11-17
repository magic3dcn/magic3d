//#include "StdAfx.h"
#include "Filter.h"
#include "flann/flann.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include <map>

namespace MagicDGP
{
    Filter::Filter()
    {
    }

    Filter::~Filter()
    {
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

    void Filter::SimpleMeshSmooth(Mesh3D* pMesh)
    {
        MagicLog << "Filter::SimpleMeshSmooth...." << std::endl;
        int vertNum = pMesh->GetVertexNumber();
        std::vector<Vector3> posBakList(vertNum);
        for (int vid = 0; vid < vertNum; vid++)
        {
            posBakList.at(vid) = pMesh->GetVertex(vid)->GetPosition();
        }
        Real smoothWeight = 0.5;
        for (int vid = 0; vid < vertNum; vid++)
        {
            Vertex3D* pVert = pMesh->GetVertex(vid);
            Edge3D* pEdge = pVert->GetEdge();
            Vector3 avgPos(0, 0, 0);
            int neighNum = 0;
            do
            {
                avgPos += posBakList.at(pEdge->GetVertex()->GetId());
                neighNum++;

                pEdge = pEdge->GetPair()->GetNext();
            } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            avgPos /= neighNum;
            avgPos = avgPos * smoothWeight + pVert->GetPosition() * (1 - smoothWeight);
            pVert->SetPosition(avgPos);
        }
        pMesh->UpdateNormal();
    }

    Point3DSet* Filter::RemovePointSetOutlier(Point3DSet* pPS, Real proportion)
    {
        int dim = 3;
        int pointNum = pPS->GetPointNumber();
        int refNum = pointNum;
        float* dataSet = new float[refNum * dim];
        int searchNum = pointNum;
        float* searchSet = new float[searchNum * dim];
        for (int i = 0; i < pointNum; i++)
        {
            MagicDGP::Vector3 pos = pPS->GetPoint(i)->GetPosition();
            MagicDGP::Vector3 nor = pPS->GetPoint(i)->GetNormal();
            dataSet[dim * i + 0] = pos[0];
            dataSet[dim * i + 1] = pos[1];
            dataSet[dim * i + 2] = pos[2];
            searchSet[dim * i + 0] = pos[0];
            searchSet[dim * i + 1] = pos[1];
            searchSet[dim * i + 2] = pos[2];
        }
        int nn = 15;
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
        std::map<float, int> densityMap;
        for (int i = 0; i < pointNum; i++)
        {
            MagicDGP::Vector3 pos = pPS->GetPoint(i)->GetPosition();
            MagicDGP::Vector3 nor = pPS->GetPoint(i)->GetNormal();
            float density = 0;
            int baseIndex = nn * i;
            for (int j = 0; j < nn; j++)
            {
                MagicDGP::Vector3 posNeigh = pPS->GetPoint(pIndex[baseIndex + j])->GetPosition();
                MagicDGP::Vector3 deltaPos = posNeigh - pos + nor * 10 * ( (posNeigh - pos) * nor );
                density += deltaPos.Length();
            }
            densityMap[density] = i;
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

        int invalidNum = pointNum * proportion;
        int invalidIndex = 0;
        for (std::map<float, int>::reverse_iterator itr = densityMap.rbegin(); itr != densityMap.rend(); ++itr)
        {
            if (invalidIndex == invalidNum)
            {
                break;
            }
            pPS->GetPoint(itr->second)->SetValid(false);
            invalidIndex++;
        }
        densityMap.clear();
        MagicDGP::Point3DSet* pNewPS = new MagicDGP::Point3DSet;
        for (int i = 0; i < pointNum; i++)
        {
            if (pPS->GetPoint(i)->IsValid() == false)
            {
                continue;
            }
            MagicDGP::Point3D* pPoint = pPS->GetPoint(i);
            MagicDGP::Point3D* pNewPoint = new MagicDGP::Point3D(pPoint->GetPosition(), pPoint->GetNormal());
            pNewPS->InsertPoint(pNewPoint);
        }

        return pNewPS;
    }

    Point3DSet* Filter::PointSetSampling(Point3DSet* pPS, int sampleNum)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        int psNum = pPS->GetPointNumber();
        if (sampleNum > psNum)
        {
            sampleNum = psNum;
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
        MagicLog << "Sampling time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart
            << " sample number: " << sampleIndex.size() << std::endl;
        MagicDGP::Point3DSet* pNewPS = new MagicDGP::Point3DSet;
        for (int sid = 0; sid < sampleNum; ++sid)
        {
            MagicDGP::Point3D* pPoint = pPS->GetPoint(sampleIndex.at(sid));
            MagicDGP::Point3D* pNewPoint = new MagicDGP::Point3D(pPoint->GetPosition(), pPoint->GetNormal());
            pNewPS->InsertPoint(pNewPoint);
        }

        return pNewPS;
    }
}
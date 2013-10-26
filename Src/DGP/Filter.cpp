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
}
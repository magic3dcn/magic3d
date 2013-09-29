#include "PrimitiveDetection.h"

namespace MagicDGP
{
    ShapeCandidate::ShapeCandidate()
    {
    }

    ShapeCandidate::~ShapeCandidate()
    {
    }

    int ShapeCandidate::GetSupportNum()
    {
        return mSupportVertex.size();
    }

    std::vector<int>& ShapeCandidate::GetSupportVertex()
    {
        return mSupportVertex;
    }

    PlaneCandidate::PlaneCandidate(const Vector3& pos0, const Vector3& pos1, const Vector3& pos2, const Vector3& nor0, const Vector3& nor1, const Vector3& nor2) :
        mPos0(pos0),
        mPos1(pos1),
        mPos2(pos2),
        mNor0(nor0),
        mNor1(nor1),
        mNor2(nor2)
    {
    }

    PlaneCandidate::~PlaneCandidate()
    {
    }

    bool PlaneCandidate::IsValid()
    {
        return true;
    }

    int PlaneCandidate::CalSupportVertex(const Mesh3D* pMesh)
    {
        return 0;
    }

    PrimitiveType PlaneCandidate::GetType()
    {
        return Plane;
    }

    SphereCandidate::SphereCandidate(const Vector3& pos0, const Vector3& pos1, const Vector3& nor0, const Vector3& nor1) :
        mPos0(pos0),
        mPos1(pos1),
        mNor0(nor0),
        mNor1(nor1)
    {
    }

    SphereCandidate::~SphereCandidate()
    {
    }

    bool SphereCandidate::IsValid()
    {
        return true;
    }

    int SphereCandidate::CalSupportVertex(const Mesh3D* pMesh)
    {
        return 0;
    }

    PrimitiveType SphereCandidate::GetType()
    {
        return Sphere;
    }

    CylinderCandidate::CylinderCandidate(const Vector3& pos0, const Vector3& pos1, const Vector3& nor0, const Vector3& nor1) :
        mPos0(pos0),
        mPos1(pos1),
        mNor0(nor0),
        mNor1(nor1)
    {
    }

    CylinderCandidate::~CylinderCandidate()
    {
    }

    bool CylinderCandidate::IsValid()
    {
        return true;
    }

    int CylinderCandidate::CalSupportVertex(const Mesh3D* pMesh)
    {
        return 0;
    }

    PrimitiveType CylinderCandidate::GetType()
    {
        return Cylinder;
    }

    ConeCandidate::ConeCandidate(const Vector3& pos0, const Vector3& pos1, const Vector3& pos2, const Vector3& nor0, const Vector3& nor1, const Vector3& nor2) :
        mPos0(pos0),
        mPos1(pos1),
        mPos2(pos2),
        mNor0(nor0),
        mNor1(nor1),
        mNor2(nor2)
    {
    }

    ConeCandidate::~ConeCandidate()
    {
    }

    bool ConeCandidate::IsValid()
    {
        return true;
    }

    int ConeCandidate::CalSupportVertex(const Mesh3D* pMesh)
    {
        return 0;
    }

    PrimitiveType ConeCandidate::GetType()
    {
        return Cone;
    }

    PrimitiveDetection::PrimitiveDetection()
    {
    }

    PrimitiveDetection::~PrimitiveDetection()
    {
    }

    void PrimitiveDetection::Primitive2DDetectionFromRansac(const Mesh3D* pMesh, std::vector<int>& res)
    {
        MagicDependence::RansacPrimitiveShapes::MeshPrimitiveDetection(pMesh, res);
    }

    void PrimitiveDetection::Primitive2DDetection(const Mesh3D* pMesh, std::vector<int>& res)
    {
        std::vector<ShapeCandidate* > candidates;
        std::vector<ShapeCandidate* > extractedShapes;
        int vertNum = pMesh->GetVertexNumber();
        std::vector<int> validFlags(vertNum, 1);
        int validNum = vertNum;
        float acceptProbability = 0.99;
        int maxIteration = 5;
        for (int k = 0; k < maxIteration; k++)
        {
            AddShapeCandidates(candidates, pMesh, validFlags);
            int bestCand = ChoseBestCandidate(candidates);
            if (IsCandidateAcceptable(bestCand, candidates))
            {
                extractedShapes.push_back(candidates.at(bestCand));
                RemoveAcceptableCandidate(bestCand, candidates);
            }
        }
        res = std::vector<int>(vertNum, 0);
        for (int i = 0; i < extractedShapes.size(); i++)
        {
            std::vector<int> supportVertex = extractedShapes.at(i)->GetSupportVertex();
            int primType = extractedShapes.at(i)->GetType();
            for (int j = 0; j < supportVertex.size(); j++)
            {
                res.at(supportVertex.at(j)) = primType;
            }
        }
    }

    void PrimitiveDetection::Primitive2DDetectionTest(const Mesh3D* pMesh, std::vector<int>& res)
    {
        //std::vector<ShapeCandidate* > candidates;
        //std::vector<ShapeCandidate* > extractedShapes;
        //int vertNum = pMesh->GetVertexNumber();
        //FindAllShapeCandidates(candidates, pMesh);
        //while (candidates.size() > 0)
        //{
        //    int bestCand = ChoseBestCandidate(candidates);
        //    if (IsCandidateAcceptable(bestCand, candidates))
        //    {
        //        extractedShapes.push_back(candidates.at(bestCand));
        //        RemoveAcceptableCandidate(bestCand, candidates);
        //    }
        //}
        //res = std::vector<int>(vertNum, 0);
        //for (int i = 0; i < extractedShapes.size(); i++)
        //{
        //    std::vector<int> supportVertex = extractedShapes.at(i)->GetSupportVertex();
        //    int primType = extractedShapes.at(i)->GetType();
        //    for (int j = 0; j < supportVertex.size(); j++)
        //    {
        //        res.at(supportVertex.at(j)) = primType;
        //    }
        //}
        static int sampleIndex = 0;
        int vertNum = pMesh->GetVertexNumber();
        res = std::vector<int>(vertNum, 0);
        int minSupportNum = 200;
        sampleIndex++;
        sampleIndex = sampleIndex % vertNum;
        while (true)
        {
            const Vertex3D* pVert = pMesh->GetVertex(sampleIndex);
            //Get vertex n neigbors
            std::vector<int> neighborList;
            std::map<int, int> visitFlag;
            std::vector<int> tranStack;
            tranStack.push_back(pVert->GetId());
            visitFlag[pVert->GetId()] = 1;
            int neighborRadius = 5;
            for (int i = 0; i < neighborRadius; i++)
            {
                std::vector<int> tranStackNext;
                for (std::vector<int>::iterator itr = tranStack.begin(); itr != tranStack.end(); ++itr)
                {
                    const Vertex3D* pVertNeig = pMesh->GetVertex(*itr);
                    const Edge3D* pEdgeNeig = pVertNeig->GetEdge();
                    do
                    {
                        if (pEdgeNeig == NULL)
                        {
                            break;
                        }
                        int newId = pEdgeNeig->GetVertex()->GetId();
                        if (visitFlag[newId] != 1)
                        {
                            visitFlag[newId] = 1;
                            tranStackNext.push_back(newId);
                            neighborList.push_back(newId);
                        }
                        pEdgeNeig = pEdgeNeig->GetPair()->GetNext();
                    } while (pEdgeNeig != pVertNeig->GetEdge());
                }
                tranStack = tranStackNext;
            }
            int neighborSize = neighborList.size();
            if (neighborSize < 2)
            {
                continue;
            }
            //
            const Vertex3D* pVertNeig0 = pMesh->GetVertex(neighborSize / 2 - 1);
            const Vertex3D* pVertNeig1 = pMesh->GetVertex(neighborSize - 1);
            //Add Plane Candidate
            ShapeCandidate* planeCand = new PlaneCandidate(pVert->GetPosition(), pVertNeig0->GetPosition(), pVertNeig1->GetPosition(),
                pVert->GetNormal(), pVertNeig0->GetNormal(), pVertNeig1->GetNormal());
            if (planeCand->IsValid())
            {
                if (planeCand->CalSupportVertex(pMesh) > minSupportNum)
                {
                    std::vector<int> supportList = planeCand->GetSupportVertex();
                    for (int i = 0; i < supportList.size(); i++)
                    {
                        res.at(supportList.at(i)) = 1;
                    }
                    res.at(pVert->GetId()) = 2;
                    break;
                }
                else
                {
                    delete planeCand;
                }
            }
            else
            {
                delete planeCand;
            }
            sampleIndex++;
            sampleIndex = sampleIndex % vertNum;
        }
    }

    void PrimitiveDetection::FindAllShapeCandidates(std::vector<ShapeCandidate* >& candidates, const Mesh3D* pMesh)
    {
        int minSupportNum = 200;
        int vertNum = pMesh->GetVertexNumber();
        for (int sampleIndex = 0; sampleIndex < vertNum; sampleIndex += 100)
        {
            const Vertex3D* pVert = pMesh->GetVertex(sampleIndex);
            //Get vertex n neigbors
            std::vector<int> neighborList;
            std::map<int, int> visitFlag;
            std::vector<int> tranStack;
            tranStack.push_back(pVert->GetId());
            visitFlag[pVert->GetId()] = 1;
            int neighborRadius = 5;
            for (int i = 0; i < neighborRadius; i++)
            {
                std::vector<int> tranStackNext;
                for (std::vector<int>::iterator itr = tranStack.begin(); itr != tranStack.end(); ++itr)
                {
                    const Vertex3D* pVertNeig = pMesh->GetVertex(*itr);
                    const Edge3D* pEdgeNeig = pVertNeig->GetEdge();
                    do
                    {
                        if (pEdgeNeig == NULL)
                        {
                            break;
                        }
                        int newId = pEdgeNeig->GetVertex()->GetId();
                        if (visitFlag[newId] != 1)
                        {
                            visitFlag[newId] = 1;
                            tranStackNext.push_back(newId);
                            neighborList.push_back(newId);
                        }
                        pEdgeNeig = pEdgeNeig->GetPair()->GetNext();
                    } while (pEdgeNeig != pVertNeig->GetEdge());
                }
                tranStack = tranStackNext;
            }
            int neighborSize = neighborList.size();
            if (neighborSize < 2)
            {
                continue;
            }
            //
            const Vertex3D* pVertNeig0 = pMesh->GetVertex(neighborSize / 2 - 1);
            const Vertex3D* pVertNeig1 = pMesh->GetVertex(neighborSize - 1);
            //Add Plane Candidate
            ShapeCandidate* planeCand = new PlaneCandidate(pVert->GetPosition(), pVertNeig0->GetPosition(), pVertNeig1->GetPosition(),
                pVert->GetNormal(), pVertNeig0->GetNormal(), pVertNeig1->GetNormal());
            if (planeCand->IsValid())
            {
                if (planeCand->CalSupportVertex(pMesh) > minSupportNum)
                {
                    candidates.push_back(planeCand);
                }
                else
                {
                    delete planeCand;
                }
            }
            else
            {
                delete planeCand;
            }
            //Add Sphere Candidate
            ShapeCandidate* sphereCand = new SphereCandidate(pVert->GetPosition(), pVertNeig1->GetPosition(),
                pVert->GetNormal(), pVertNeig1->GetNormal());
            if (sphereCand->IsValid())
            {
                if (sphereCand->CalSupportVertex(pMesh) > minSupportNum)
                {
                    candidates.push_back(sphereCand);
                }
                else
                {
                    delete sphereCand;
                }
            }
            else
            {
                delete sphereCand;
            }
            //Add Cylinder Candidate

            //Add Cone Candidate
        }
    }

    void PrimitiveDetection::AddShapeCandidates(std::vector<ShapeCandidate* >& candidates, const Mesh3D* pMesh, std::vector<int>& validFlags)
    {
        int iterNum = 200;

        for (int candIter = 0; candIter < iterNum; candIter++)
        {
            
        }
    }

    int PrimitiveDetection::ChoseBestCandidate(std::vector<ShapeCandidate* >& candidates)
    {

    }

    bool PrimitiveDetection::IsCandidateAcceptable(int index, std::vector<ShapeCandidate* >& candidates)
    {
        return true;
    }

    void PrimitiveDetection::RemoveAcceptableCandidate(int index, std::vector<ShapeCandidate* >& candidates)
    {

    }
}
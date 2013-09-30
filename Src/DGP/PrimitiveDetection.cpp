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

    //PlaneCandidate::PlaneCandidate(const Vector3& pos0, const Vector3& pos1, const Vector3& pos2, const Vector3& nor0, const Vector3& nor1, const Vector3& nor2) :
    //    mPos0(pos0),
    //    mPos1(pos1),
    //    mPos2(pos2),
    //    mNor0(nor0),
    //    mNor1(nor1),
    //    mNor2(nor2)
    //{
    //}

    PlaneCandidate::PlaneCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1, const Vertex3D* pVert2) :
        mpVert0(pVert0),
        mpVert1(pVert1),
        mpVert2(pVert2)
    {

    }

    PlaneCandidate::~PlaneCandidate()
    {
    }

    bool PlaneCandidate::IsValid()
    {
        //Calculate parameter
        Vector3 pos0 = mpVert0->GetPosition();
        Vector3 pos1 = mpVert1->GetPosition();
        Vector3 pos2 = mpVert2->GetPosition();
        mCenter = (pos0 + pos1 + pos2) / 3; // in fact, mCenter can equal any pos!
        Vector3 dir0 = pos1 - pos0;
        Vector3 dir1 = pos2 - pos0;
        mNormal = dir0.CrossProduct(dir1);
        if (mNormal.Normalise() < Epsilon)
        {
            MagicLog << "mNormal Length too small" << std::endl; 
            return false;
        }

        //Judge whether valid
        Real MaxAngleDeviation = 0.9848;
        if (fabs(mNormal * (mpVert0->GetNormal()) ) < MaxAngleDeviation || 
            fabs(mNormal * (mpVert1->GetNormal()) ) < MaxAngleDeviation ||
            fabs(mNormal * (mpVert2->GetNormal()) ) < MaxAngleDeviation)
        {
            MagicLog << "Angle Deviation: " << mNormal * (mpVert0->GetNormal()) << " " << mNormal * (mpVert1->GetNormal()) 
                << " " << mNormal * (mpVert2->GetNormal()) << std::endl;
            return false;
        }
        else
        {
            return true;
        }
    }

    int PlaneCandidate::CalSupportVertex(const Mesh3D* pMesh)
    {
        Real MaxAngleDeviation = 0.9848;
        Real MaxDistDeviation = 0.001;
        int id0 = mpVert0->GetId();
        int id1 = mpVert1->GetId();
        int id2 = mpVert2->GetId();
        mSupportVertex.clear();
        mSupportVertex.push_back(id0);
        mSupportVertex.push_back(id1);
        mSupportVertex.push_back(id2);
        std::map<int, int> visitFlag;
        visitFlag[id0] = 1;
        visitFlag[id1] = 1;
        visitFlag[id2] = 1;
        std::vector<int> searchIndex;
        searchIndex.push_back(id0);
        searchIndex.push_back(id1);
        searchIndex.push_back(id2);
        while (searchIndex.size() > 0)
        {
            MagicLog << "Search support vertex: searchIndex size: " << searchIndex.size() << std::endl;
            std::vector<int> searchIndexNext;
            for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
            {
                const Vertex3D* pVert = pMesh->GetVertex(*itr);
                const Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    const Vertex3D* pNewVert = pEdge->GetVertex();
                    int newId = pNewVert->GetId();
                    if (visitFlag[newId] != 1)
                    {
                        visitFlag[newId] = 1;
                        Vector3 pos = pNewVert->GetPosition();
                        Real distDev = fabs( (pos - mCenter) * mNormal );
                        if (distDev < MaxDistDeviation)
                        {
                            Vector3 nor = pNewVert->GetNormal();
                            Real angleDev = fabs( nor * mNormal );
                            if (angleDev > MaxAngleDeviation)
                            {
                                searchIndexNext.push_back(newId);
                                mSupportVertex.push_back(newId);
                            }
                        }
                    }

                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }

        MagicLog << "Support vertex size: " << mSupportVertex.size() << std::endl;

        return mSupportVertex.size();
    }

    PrimitiveType PlaneCandidate::GetType()
    {
        return Plane;
    }

    SphereCandidate::SphereCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1) :
        mpVert0(pVert0),
        mpVert1(pVert1)
    {
    }

    SphereCandidate::~SphereCandidate()
    {
    }

    bool SphereCandidate::IsValid()
    {
        Real MaxAngleDeviation = 0.9848;
        Real MaxDistDeviation = 0.001;
        Vector3 nor0 = mpVert0->GetNormal();
        Vector3 nor1 = mpVert1->GetNormal();
        Real cosTheta = nor0 * nor1;
        Real cosThetaSquare = cosTheta * cosTheta;
        if (1 - cosThetaSquare < Epsilon)
        {
            MagicLog << "cosThetaSquare too large: " << cosThetaSquare << std::endl;
            return false;
        }
        Vector3 pos0 = mpVert0->GetPosition();
        Vector3 pos1 = mpVert1->GetPosition();
        Vector3 dir = pos1 - pos0;
        Real b0 = dir * nor0;
        Real b1 = dir * nor1;
        Real t0 = (b0 - b1 * cosTheta) / (1 - cosThetaSquare);
        Real t1 = t0 * cosTheta - b1;
        Vector3 interPos0 = pos0 + nor0 * t0;
        Vector3 interPos1 = pos1 + nor1 * t1;
        mCenter = (interPos0 + interPos1) / 2;
        mRadius = ( (pos0 - mCenter).Length() + (pos1 - mCenter).Length() ) / 2;
        //Judge
        Vector3 dir0 = pos0 - mCenter;
        Real dist0 = dir0.Normalise();
        if (fabs(dist0 - mRadius) > MaxDistDeviation || fabs(dir0 * nor0) < MaxAngleDeviation)
        {
            MagicLog << "Sphere is valid reject vertex0: " << fabs(dist0 - mRadius) << " " << fabs(dir0 * nor0) << std::endl;
            return false;
        }
        Vector3 dir1 = pos1 - mCenter;
        Real dist1 = dir1.Normalise();
        if (fabs(dist1 - mRadius) > MaxDistDeviation || fabs(dir1 * nor1) < MaxAngleDeviation)
        {
            MagicLog << "Sphere is valid reject vertex1: " << fabs(dist1 - mRadius) << " " << fabs(dir1 * nor1) << std::endl;
            return false;
        }
        MagicLog << "Sphere: " << mCenter[0] << " " << mCenter[1] << " " << mCenter[2] << " " << mRadius << std::endl; 
        return true;
    }

    int SphereCandidate::CalSupportVertex(const Mesh3D* pMesh)
    {
        Real MaxAngleDeviation = 0.9848;
        Real MaxDistDeviation = 0.001;
        int id0 = mpVert0->GetId();
        int id1 = mpVert1->GetId();
        mSupportVertex.clear();
        mSupportVertex.push_back(id0);
        mSupportVertex.push_back(id1);
        std::map<int, int> visitFlag;
        visitFlag[id0] = 1;
        visitFlag[id1] = 1;
        std::vector<int> searchIndex;
        searchIndex.push_back(id0);
        searchIndex.push_back(id1);
        while (searchIndex.size() > 0)
        {
            //MagicLog << "Search support vertex: searchIndex size: " << searchIndex.size() << std::endl;
            std::vector<int> searchIndexNext;
            for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
            {
                const Vertex3D* pVert = pMesh->GetVertex(*itr);
                const Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    const Vertex3D* pNewVert = pEdge->GetVertex();
                    int newId = pNewVert->GetId();
                    if (visitFlag[newId] != 1)
                    {
                        visitFlag[newId] = 1;
                        Vector3 pos = pNewVert->GetPosition();
                        Vector3 nor = pNewVert->GetNormal();
                        Vector3 dir = pos - mCenter;
                        Real length = dir.Normalise();
                        if (fabs(length - mRadius) < MaxDistDeviation && fabs(dir * nor) > MaxAngleDeviation)
                        {
                            searchIndexNext.push_back(newId);
                            mSupportVertex.push_back(newId);
                        }
                    }

                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }

        MagicLog << "Support vertex size: " << mSupportVertex.size() << std::endl;

        return mSupportVertex.size();
    }

    PrimitiveType SphereCandidate::GetType()
    {
        return Sphere;
    }

    CylinderCandidate::CylinderCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1) :
        mpVert0(pVert0),
        mpVert1(pVert1)
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
        Real MaxAngleDeviation = 0.9848;
        Real MaxDistDeviation = 0.001;
        int id0 = mpVert0->GetId();
        int id1 = mpVert1->GetId();
        mSupportVertex.clear();
        mSupportVertex.push_back(id0);
        mSupportVertex.push_back(id1);
        std::map<int, int> visitFlag;
        visitFlag[id0] = 1;
        visitFlag[id1] = 1;
        std::vector<int> searchIndex;
        searchIndex.push_back(id0);
        searchIndex.push_back(id1);
        while (searchIndex.size() > 0)
        {
            //MagicLog << "Search support vertex: searchIndex size: " << searchIndex.size() << std::endl;
            std::vector<int> searchIndexNext;
            for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
            {
                const Vertex3D* pVert = pMesh->GetVertex(*itr);
                const Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    const Vertex3D* pNewVert = pEdge->GetVertex();
                    int newId = pNewVert->GetId();
                    if (visitFlag[newId] != 1)
                    {
                        visitFlag[newId] = 1;
                        Vector3 pos = pNewVert->GetPosition();
                        Vector3 projectPos = pos + mDir * ((mCenter - pos) * mDir);
                        Vector3 nor = pNewVert->GetNormal();
                        Vector3 dir = projectPos - mCenter;
                        Real length = dir.Normalise();
                        if (fabs(length - mRadius) < MaxDistDeviation && fabs(dir * nor) > MaxAngleDeviation)
                        {
                            searchIndexNext.push_back(newId);
                            mSupportVertex.push_back(newId);
                        }
                    }

                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }

        MagicLog << "Support vertex size: " << mSupportVertex.size() << std::endl;

        return mSupportVertex.size();
    }

    PrimitiveType CylinderCandidate::GetType()
    {
        return Cylinder;
    }

    ConeCandidate::ConeCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1, const Vertex3D* pVert2) :
        mpVert0(pVert0),
        mpVert1(pVert1),
        mpVert2(pVert2)
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
        MagicLog << "PrimitiveDetection::Primitive2DDetectionTest begin" << std::endl;
        static int sampleIndex = 0;
        int vertNum = pMesh->GetVertexNumber();
        res = std::vector<int>(vertNum, 0);
        int minSupportNum = 200;
        sampleIndex += 1001;
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
            if (neighborSize < 4)
            {
                continue;
            }
            //test
            //for (int i = 0; i < neighborSize; i++)
            //{
            //    res.at(neighborList.at(i)) = 1;
            //}
            //res.at(neighborList.at(neighborSize / 3)) = 2;
            //res.at(pVert->GetId()) = 2;
            //res.at(neighborList.at(neighborSize - 1)) = 2;
            //break;
            //
            //
            const Vertex3D* pVertNeig0 = pMesh->GetVertex( neighborList.at(neighborSize / 3) );
            const Vertex3D* pVertNeig1 = pMesh->GetVertex( neighborList.at(neighborSize - 1) );
            //Add Plane Candidate
            /*ShapeCandidate* planeCand = new PlaneCandidate(pVert, pVertNeig0, pVertNeig1);
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
            }*/
            //Add Sphere Candidate
            ShapeCandidate* sphereCand = new SphereCandidate(pVert, pVertNeig1);
            if (sphereCand->IsValid())
            {
                //if (sphereCand->CalSupportVertex(pMesh) > minSupportNum)
                if (sphereCand->CalSupportVertex(pMesh) > 100)
                {
                    std::vector<int> supportList = sphereCand->GetSupportVertex();
                    for (int i = 0; i < supportList.size(); i++)
                    {
                        res.at(supportList.at(i)) = 3;
                    }
                    res.at(pVert->GetId()) = 2;
                    res.at(pVertNeig1->GetId()) = 2;
                    break;
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
            //
            sampleIndex += 1001;
            sampleIndex = sampleIndex % vertNum;
        }
        MagicLog << "PrimitiveDetection::Primitive2DDetectionTest End" << std::endl;
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
            //ShapeCandidate* planeCand = new PlaneCandidate(pVert->GetPosition(), pVertNeig0->GetPosition(), pVertNeig1->GetPosition(),
            //    pVert->GetNormal(), pVertNeig0->GetNormal(), pVertNeig1->GetNormal());
            ShapeCandidate* planeCand = new PlaneCandidate(pVert, pVertNeig0, pVertNeig1);
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
            /*ShapeCandidate* sphereCand = new SphereCandidate(pVert->GetPosition(), pVertNeig1->GetPosition(),
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
            }*/
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
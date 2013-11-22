#include "PrimitiveDetection.h"
#include "Eigen/Dense"
#include "../Common/ToolKit.h"

namespace MagicDGP
{
    static int minInitSupportNum = 10;
    static int minSupportNum = 100;
    static Real minSupportArea = 1;
    static Real acceptableAreaScale = 0.17;
    static Real acceptableArea = 1;
    static Real acceptableAreaDelta = 1;
    static Real maxAngleDeviation = 0.866;
    static Real maxDistDeviation = 0.01;
    static Real maxCylinderRadiusScale = 0.2;
    static Real maxSphereRadiusScale = 0.01;
    static Real maxSphereRadius = 1;
    static Real maxCylinderRadius = 1;
    static Real minConeAngle = 0.1745329251994329; //10 degree
    static Real maxConeAngle = 1.3962; //80 degree
    static Real maxConeAngleDeviation = 0.1745; //10 degree
    //static Real baseScore = 0.93969262;
    static Real baseScore = 0.2618;

    ShapeCandidate::ShapeCandidate() :
        mScore(0),
        mRemoved(false),
        mSupportArea(0),
        mHasRefit(false)
    {
    }

    ShapeCandidate::~ShapeCandidate()
    {
    }

    bool ShapeCandidate::IsRemoved()
    {
        return mRemoved;
    }

    void ShapeCandidate::SetRemoved(bool accepted)
    {
        mRemoved = accepted;
    }

    int ShapeCandidate::GetSupportNum()
    {
        return mSupportVertex.size();
    }

    void ShapeCandidate::SetSupportVertex(const std::vector<int>& supportVertex)
    {
        mSupportVertex = supportVertex;
    }

    Real ShapeCandidate::GetSupportArea()
    {
        return mSupportArea;
    }

    void ShapeCandidate::UpdateSupportArea(const Mesh3D* pMesh, std::vector<Real>& vertWeightList)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        mSupportArea = 0;
        for (std::vector<int>::iterator itr = mSupportVertex.begin(); itr != mSupportVertex.end(); ++itr)
        {
            mSupportArea += vertWeightList.at(*itr);
        }
        mSupportArea /= 3;
        //MagicLog << "UpdateSupportArea time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
    }

    std::vector<int>& ShapeCandidate::GetSupportVertex()
    {
        return mSupportVertex;
    }

    Real ShapeCandidate::GetScore()
    {
        return mScore;
    }

    bool ShapeCandidate::HasRefit() const
    {
        return mHasRefit;
    }

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
            //MagicLog << "mNormal Length too small" << std::endl; 
            return false;
        }

        //Judge whether valid
        //Real MaxAngleDeviation = 0.9848;
        if (fabs(mNormal * (mpVert0->GetNormal()) ) < maxAngleDeviation || 
            fabs(mNormal * (mpVert1->GetNormal()) ) < maxAngleDeviation ||
            fabs(mNormal * (mpVert2->GetNormal()) ) < maxAngleDeviation)
        {
            //MagicLog << "Angle Deviation: " << mNormal * (mpVert0->GetNormal()) << " " << mNormal * (mpVert1->GetNormal()) 
            //    << " " << mNormal * (mpVert2->GetNormal()) << std::endl;
            return false;
        }
        else
        {
            return true;
        }
    }

    int PlaneCandidate::CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        //Real MaxAngleDeviation = 0.9848;
        //Real MaxAngleDeviation = 0.94;
        //Real MaxDistDeviation = 0.001;
        int id0 = mpVert0->GetId();
        int id1 = mpVert1->GetId();
        int id2 = mpVert2->GetId();
        mSupportVertex.clear();
        mSupportVertex.push_back(id0);
        mSupportVertex.push_back(id1);
        mSupportVertex.push_back(id2);
        //std::map<int, int> visitFlag;
        std::vector<bool> visitFlag(pMesh->GetVertexNumber(), 0);
        visitFlag[id0] = 1;
        visitFlag[id1] = 1;
        visitFlag[id2] = 1;
        std::vector<int> searchStack[2];
        searchStack[0].push_back(id0);
        searchStack[0].push_back(id1);
        searchStack[0].push_back(id2);
        int stackCurrent = 0;
        int stackNext = 1;
        //std::vector<int> searchIndex;
        //searchIndex.push_back(id0);
        //searchIndex.push_back(id1);
        //searchIndex.push_back(id2);
        //while (searchIndex.size() > 0)
        while (searchStack[stackCurrent].size() > 0)
        {
            //MagicLog << "Search support vertex: searchIndex size: " << searchIndex.size() << std::endl;
            //std::vector<int> searchIndexNext;
            searchStack[stackNext].clear();
            for (std::vector<int>::iterator itr = searchStack[stackCurrent].begin(); itr != searchStack[stackCurrent].end(); ++itr)
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
                        if (resFlag.at(newId) == PrimitiveType::None)
                        {
                            Vector3 pos = pNewVert->GetPosition();
                            Real distDev = fabs( (pos - mCenter) * mNormal );
                            if (distDev < maxDistDeviation)
                            {
                                Vector3 nor = pNewVert->GetNormal();
                                Real angleDev = fabs( nor * mNormal );
                                if (angleDev > maxAngleDeviation)
                                {
                                    //searchIndexNext.push_back(newId);
                                    searchStack[stackNext].push_back(newId);
                                    mSupportVertex.push_back(newId);
                                }
                            }
                        }
                    }

                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            //searchIndex = searchIndexNext;
            int indexTemp = stackCurrent;
            stackCurrent = stackNext;
            stackNext = indexTemp;
        }

        //MagicLog << "Support vertex size: " << mSupportVertex.size() << std::endl;
       // MagicLog << "Plane Support Vertex time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
        return mSupportVertex.size();
    }

    int PlaneCandidate::Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag)
    {
        mHasRefit = true;
        //Refit parameter
        mCenter = Vector3(0, 0, 0);
        mNormal = Vector3(0, 0, 0);
        for (std::vector<int>::iterator itr = mSupportVertex.begin(); itr != mSupportVertex.end(); ++itr)
        {
            const Vertex3D* pVert = pMesh->GetVertex(*itr);
            mCenter += pVert->GetPosition();
            mNormal += pVert->GetNormal();
        }
        int supportSize = mSupportVertex.size();
        mCenter /= supportSize;
        mNormal.Normalise();
        //refit supprot vertex
        //Real MaxAngleDeviation = 0.9848;
        //Real MaxAngleDeviation = 0.94;
        //Real MaxDistDeviation = 0.001 * 5;
        //std::map<int, int> visitFlag;
        std::vector<bool> visitFlag(pMesh->GetVertexNumber(), 0);
        std::vector<int> searchIndex = mSupportVertex;
        for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
        {
            visitFlag[*itr] = 1;
        }
        mSupportVertex.clear();
        //mScore = 0;
        while (searchIndex.size() > 0)
        {
            std::vector<int> searchIndexNext;
            for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
            {
                //first check current vertex
                const Vertex3D* pVert = pMesh->GetVertex(*itr);
                Vector3 pos = pVert->GetPosition();
                Real distDev = fabs( (pos - mCenter) * mNormal );
                if (distDev > maxDistDeviation)
                {
                    continue;
                }
                Vector3 nor = pVert->GetNormal();
                Real angleDev = fabs( nor * mNormal );
                if (angleDev < maxAngleDeviation)
                {
                    continue;
                }
                //mScore += angleDev - baseScore;
                mSupportVertex.push_back(*itr);
                //if current vertex pass, push its neighbors into searchIndexNext
                const Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    int newId = pEdge->GetVertex()->GetId();
                    if (visitFlag[newId] != 1)
                    {
                        visitFlag[newId] = 1;
                        if (resFlag.at(newId) == PrimitiveType::None)
                        {
                            searchIndexNext.push_back(newId);
                        }
                    }
                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }

        //MagicLog << "Refit Support vertex size: " << mSupportVertex.size() << std::endl;

        return mSupportVertex.size();
    }

    PrimitiveType PlaneCandidate::GetType()
    {
        return Plane;
    }

    void PlaneCandidate::UpdateScore(const Mesh3D* pMesh, std::vector<Real>& vertWeightList)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        mScore = 0;
        int supportNum = mSupportVertex.size();
        for (std::vector<int>::iterator itr = mSupportVertex.begin(); itr != mSupportVertex.end(); ++itr)
        {
            Vector3 nor = pMesh->GetVertex(*itr)->GetNormal();
            //mScore += (fabs(nor * mNormal) - baseScore) * vertWeightList.at(*itr);
            Real cosA = fabs(nor * mNormal);
            cosA = cosA > 1 ? 1 : cosA;
            /*Real scoreTrunc = baseScore - acos(cosA);
            scoreTrunc = scoreTrunc > 0 ? scoreTrunc : 0;
            mScore += scoreTrunc * vertWeightList.at(*itr);*/
            mScore += (baseScore - acos(cosA)) * vertWeightList.at(*itr);
        }
        //MagicLog << "Plane Update Score: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
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
        Vector3 nor0 = mpVert0->GetNormal();
        Vector3 nor1 = mpVert1->GetNormal();
        Real cosTheta = nor0 * nor1;
        Real cosThetaSquare = cosTheta * cosTheta;
        if (1 - cosThetaSquare < Epsilon)
        {
            //MagicLog << "cosThetaSquare too large: " << cosThetaSquare << std::endl;
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
        if (mRadius > maxSphereRadius)
        {
            //MagicLog << "Sphere radius is too large: " << mRadius << std::endl;
            return false;
        }
        //Judge
        //Real MaxAngleDeviation = 0.94;
        Real MaxDistDeviation = mRadius * maxSphereRadiusScale;
        Vector3 dir0 = pos0 - mCenter;
        Real dist0 = dir0.Normalise();
        if (fabs(dist0 - mRadius) > MaxDistDeviation || fabs(dir0 * nor0) < maxAngleDeviation)
        {
            //MagicLog << "Sphere is valid reject vertex0: " << fabs(dist0 - mRadius) << " " << fabs(dir0 * nor0) << std::endl;
            return false;
        }
        Vector3 dir1 = pos1 - mCenter;
        Real dist1 = dir1.Normalise();
        if (fabs(dist1 - mRadius) > MaxDistDeviation || fabs(dir1 * nor1) < maxAngleDeviation)
        {
            //MagicLog << "Sphere is valid reject vertex1: " << fabs(dist1 - mRadius) << " " << fabs(dir1 * nor1) << std::endl;
            return false;
        }
        //MagicLog << "Sphere: " << mCenter[0] << " " << mCenter[1] << " " << mCenter[2] << " " << mRadius << std::endl; 
        return true;
    }

    int SphereCandidate::CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        Real MaxDistDeviation = mRadius * maxSphereRadiusScale;
        int id0 = mpVert0->GetId();
        int id1 = mpVert1->GetId();
        mSupportVertex.clear();
        mSupportVertex.push_back(id0);
        mSupportVertex.push_back(id1);
        //std::map<int, int> visitFlag;
        std::vector<bool> visitFlag(pMesh->GetVertexNumber(), 0);
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
                        if (resFlag.at(newId) == PrimitiveType::None)
                        {
                            Vector3 pos = pNewVert->GetPosition();
                            Vector3 nor = pNewVert->GetNormal();
                            Vector3 dir = pos - mCenter;
                            Real length = dir.Normalise();
                            if (fabs(length - mRadius) < MaxDistDeviation && fabs(dir * nor) > maxAngleDeviation)
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

        //MagicLog << "Support vertex size: " << mSupportVertex.size() << std::endl;
        //MagicLog << "Sphere Support Vertex time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
        return mSupportVertex.size();
    }

    int SphereCandidate::Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag)
    {
        mHasRefit = true;
        //Refit parameter
        int supportNum = mSupportVertex.size();
        Eigen::MatrixXd matA(supportNum, 4);
        Eigen::VectorXd vecB(supportNum, 1);
        for (int i = 0; i < supportNum; i++)
        {
            const Vertex3D* pVert = pMesh->GetVertex( mSupportVertex.at(i) );
            Vector3 pos = pVert->GetPosition();
            Vector3 nor = pVert->GetNormal();
            matA(i, 0) = nor[0];
            matA(i, 1) = nor[1];
            matA(i, 2) = nor[2];
            matA(i, 3) = 1;
            vecB(i) = pos * nor;
        }
        Eigen::MatrixXd matAT = matA.transpose();
        Eigen::MatrixXd matCoefA = matAT * matA;
        Eigen::MatrixXd vecCoefB = matAT * vecB;
        Eigen::VectorXd res = matCoefA.ldlt().solve(vecCoefB);
        mCenter = Vector3(res(0), res(1), res(2));
        mRadius = fabs(res(3));
        if (mRadius > maxSphereRadius)
        {
            mSupportVertex.clear();
            return 0;
        }
        //MagicLog << "Refit sphere: " << mCenter[0] << " " << mCenter[1] << " " << mCenter[2] << " " << mRadius << std::endl;
        //Refit support vertex
        //Real MaxAngleDeviation = 0.94;
        //Real MaxDistDeviation = 0.001 * 10;
        Real MaxDistDeviation = mRadius * maxSphereRadiusScale;
        //std::map<int, int> visitFlag;
        std::vector<bool> visitFlag(pMesh->GetVertexNumber(), 0);
        std::vector<int> searchIndex = mSupportVertex;
        for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
        {
            visitFlag[*itr] = 1;
        }
        mSupportVertex.clear();
        //mScore = 0;
        while (searchIndex.size() > 0)
        {
            std::vector<int> searchIndexNext;
            for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
            {
                //first check current vertex
                const Vertex3D* pVert = pMesh->GetVertex(*itr);
                Vector3 pos = pVert->GetPosition();
                Vector3 nor = pVert->GetNormal();
                Vector3 dir = pos - mCenter;
                Real length = dir.Normalise();
                Real angleDev = fabs(dir * nor);
                if (fabs(length - mRadius) > MaxDistDeviation || angleDev < maxAngleDeviation)
                {
                    continue;
                }
               // mScore += (angleDev - baseScore);
                mSupportVertex.push_back(*itr);
                //if current vertex pass, push its neighbors into searchIndexNext
                const Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    int newId = pEdge->GetVertex()->GetId();
                    if (visitFlag[newId] != 1)
                    {
                        visitFlag[newId] = 1;
                        if (resFlag.at(newId) == PrimitiveType::None)
                        {
                            searchIndexNext.push_back(newId);
                        }
                    }
                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }

        //MagicLog << "Refit Support vertex size: " << mSupportVertex.size() << std::endl;

        return mSupportVertex.size();
    }

    PrimitiveType SphereCandidate::GetType()
    {
        return Sphere;
    }

    void SphereCandidate::UpdateScore(const Mesh3D* pMesh, std::vector<Real>& vertWeightList)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        mScore = 0;
        for (std::vector<int>::iterator itr = mSupportVertex.begin(); itr != mSupportVertex.end(); ++itr)
        {
            Vector3 nor = pMesh->GetVertex(*itr)->GetNormal();
            Vector3 dir = pMesh->GetVertex(*itr)->GetPosition() - mCenter;
            dir.Normalise();
            //mScore += (fabs(nor * dir) - baseScore) * vertWeightList.at(*itr);
            Real cosA = fabs(nor * dir);
            cosA = cosA > 1 ? 1 : cosA;
            /*Real scoreTrunc = baseScore - acos(cosA);
            scoreTrunc = scoreTrunc > 0 ? scoreTrunc : 0;
            mScore += scoreTrunc * vertWeightList.at(*itr);*/
            mScore += (baseScore - acos(cosA)) * vertWeightList.at(*itr);
        }
        //MagicLog << "Sphere Update Score time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
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
        //Real MaxDistDeviation = 0.001;
        Vector3 nor0 = mpVert0->GetNormal();
        Vector3 nor1 = mpVert1->GetNormal();
        mDir = nor0.CrossProduct(nor1);
        Real dirLen = mDir.Normalise();
        if (dirLen < Epsilon)
        {
            //MagicLog << "dirLen is too small: " << dirLen << std::endl;
            return false;
        }
        Vector3 dirX = nor0;
        Vector3 dirY = mDir.CrossProduct(dirX);
        dirY.Normalise();
        Real nor1ProjectX = dirX * nor1;
        Real nor1ProjectY = dirY * nor1;
        if (fabs(nor1ProjectY) < Epsilon)
        {
            //MagicLog << "nor1ProjectY is too small: " << nor1ProjectY << std::endl;
            return false;
        }
        Vector3 pos0 = mpVert0->GetPosition();
        Vector3 pos1 = mpVert1->GetPosition();
        Vector3 originPos = pos0;
        Vector3 pos1Dir = pos1 - originPos;
        Real pos1ProjectX = dirX * pos1Dir;
        Real pos1ProjectY = dirY * pos1Dir;
        Real interX = pos1ProjectX - nor1ProjectX * pos1ProjectY / nor1ProjectY;
        mCenter = originPos + dirX * interX;
        mRadius = fabs(interX);
        Real radius2 = sqrt(pos1ProjectY * pos1ProjectY + (pos1ProjectX - interX) * (pos1ProjectX - interX));
        //if (fabs(radius2 - mRadius) / mRadius > 0.05)
        //{
        //    //MagicLog << "Radius are too different: " << radius2 - mRadius << std::endl;
        //    return false;
        //}
        mRadius = (mRadius + radius2) / 2;
        if (mRadius > maxCylinderRadius)
        {
            //MagicLog << "Cylinder radius is too large: " << mRadius << std::endl;
            return false;
        }
        //MagicLog << "Cylinder is Valid: " << mDir[0] << " " << mDir[1] << " " << mDir[2] << " " << mRadius << " " 
        //    << mCenter[0] << " " << mCenter[1] << " " << mCenter[2] << std::endl;
        //reject condition: No condition

        return true;
    }

    int CylinderCandidate::CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        Real MaxDistDeviation = mRadius * maxCylinderRadiusScale;
        int id0 = mpVert0->GetId();
        int id1 = mpVert1->GetId();
        mSupportVertex.clear();
        mSupportVertex.push_back(id0);
        mSupportVertex.push_back(id1);
        //std::map<int, int> visitFlag;
        std::vector<bool> visitFlag(pMesh->GetVertexNumber(), 0);
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
                        if (resFlag.at(newId) == PrimitiveType::None)
                        {
                            Vector3 pos = pNewVert->GetPosition();
                            Vector3 projectPos = pos + mDir * ((mCenter - pos) * mDir);
                            Vector3 nor = pNewVert->GetNormal();
                            Vector3 dir = projectPos - mCenter;
                            Real length = dir.Normalise();
                            if (fabs(length - mRadius) < MaxDistDeviation && fabs(dir * nor) > maxAngleDeviation)
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

        //MagicLog << "Support vertex size: " << mSupportVertex.size() << std::endl;
        //MagicLog << "Cylinder Support Vertex time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart 
        //    << " SupportVertexSize: " << mSupportVertex.size() << std::endl;
        return mSupportVertex.size();
    }

    int CylinderCandidate::Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag)
    {
        mHasRefit = true;
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        //Refit parameter
        int supportNum = mSupportVertex.size();
        Real XX = 0;
        Real YY = 0;
        Real ZZ = 0;
        Real XY = 0;
        Real YZ = 0;
        Real ZX = 0;
        Real X = 0;
        Real Y = 0;
        Real Z = 0;
        for (int i = 0; i < supportNum; i++)
        {
            Vector3 nor = pMesh->GetVertex(mSupportVertex.at(i))->GetNormal();
            XX += nor[0] * nor[0];
            YY += nor[1] * nor[1];
            ZZ += nor[2] * nor[2];
            XY += nor[0] * nor[1];
            YZ += nor[1] * nor[2];
            ZX += nor[2] * nor[0];
            X += nor[0];
            Y += nor[1];
            Z += nor[2];
        }
        XX /= supportNum;
        YY /= supportNum;
        ZZ /= supportNum;
        XY /= supportNum;
        YZ /= supportNum;
        ZX /= supportNum;
        X /= supportNum;
        Y /= supportNum;
        Z /= supportNum;
        Eigen::Matrix3d mat;
        mat(0, 0) = XX - X * X;
        mat(0, 1) = 2 * (XY - X * Y);
        mat(0, 2) = 2 * (ZX - Z * X);
        mat(1, 0) = 2 * (XY - X * Y);
        mat(1, 1) = YY - Y * Y;
        mat(1, 2) = 2 * (YZ - Y * Z);
        mat(2, 0) = 2 * (ZX - Z * X);
        mat(2, 1) = 2 * (YZ - Y * Z);
        mat(2, 2) = ZZ - Z * Z;
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> es;
        es.compute(mat);
        int rightEigenIndex = 0;
        Eigen::Vector3d dirVec[3];
        dirVec[0] = es.eigenvectors().col(0);
        Real eigenAngle = fabs(mDir[0] * dirVec[0](0) + mDir[1] * dirVec[0](1) + mDir[2] * dirVec[0](2));
        dirVec[1] = es.eigenvectors().col(1);
        Real eigenAngleTemp = fabs(mDir[0] * dirVec[1](0) + mDir[1] * dirVec[1](1) + mDir[2] * dirVec[1](2));
        if ( eigenAngleTemp > eigenAngle)
        {
            rightEigenIndex = 1;
            eigenAngle = eigenAngleTemp;
        }
        dirVec[2] = es.eigenvectors().col(2);
        eigenAngleTemp = fabs(mDir[0] * dirVec[2](0) + mDir[1] * dirVec[2](1) + mDir[2] * dirVec[2](2));
        if (eigenAngleTemp > eigenAngle)
        {
            rightEigenIndex = 2;
        }
        //MagicLog << "Eigne Value: " << std::endl << es.eigenvalues() << std::endl;
        //MagicLog << "Eigen Vector: " << std::endl << es.eigenvectors() << std::endl;
        if (rightEigenIndex != 0)
        {
            mSupportVertex.clear();
            return 0;
        }
        mDir = Vector3(dirVec[rightEigenIndex](0), dirVec[rightEigenIndex](1), dirVec[rightEigenIndex](2));
        mDir.Normalise();
        Vector3 planePos = pMesh->GetVertex(mSupportVertex.at(0))->GetPosition();
        Vector3 planeNor = pMesh->GetVertex(mSupportVertex.at(0))->GetNormal();
        Vector3 dirX = planeNor - mDir * (mDir * planeNor);
        dirX.Normalise();
        Vector3 dirY = mDir.CrossProduct(dirX);
        dirY.Normalise();
        Eigen::MatrixXd matA(supportNum, 3);
        Eigen::VectorXd vecB(supportNum, 1);
        for (int i = 0; i < supportNum; i++)
        {
            const Vertex3D* pVert = pMesh->GetVertex( mSupportVertex.at(i) );
            Vector3 pos = pVert->GetPosition();
            Vector3 projectPos = pos + mDir * ( mDir * (planePos - pos) );
            Vector3 projectDir = projectPos - planePos;
            Real projectX = projectDir * dirX;
            Real projectY = projectDir * dirY;
            matA(i, 0) = projectX;
            matA(i, 1) = projectY;
            matA(i, 2) = 1;
            vecB(i) = -(projectX * projectX + projectY * projectY);
        }
        Eigen::MatrixXd matAT = matA.transpose();
        Eigen::MatrixXd matCoefA = matAT * matA;
        Eigen::MatrixXd vecCoefB = matAT * vecB;
        Eigen::VectorXd res = matCoefA.ldlt().solve(vecCoefB);
        Real centerX = res(0) / -2;
        Real centerY = res(1) / -2;
        mRadius = sqrt( centerX * centerX + centerY * centerY - res(2) );
        mCenter = planePos + dirX * centerX + dirY * centerY;
        /*Eigen::MatrixXd matA(supportNum + 1, 4);
        Eigen::VectorXd vecB(supportNum + 1, 1);
        Real weight = supportNum;
        for (int i = 0; i < supportNum; i++)
        {
            const Vertex3D* pVert = pMesh->GetVertex( mSupportVertex.at(i) );
            Vector3 pos = pVert->GetPosition();
            Vector3 projectPos = pos + mDir * ( mDir * (planePos - pos) );
            Vector3 nor = pVert->GetNormal();
            matA(i, 0) = nor[0];
            matA(i, 1) = nor[1];
            matA(i, 2) = nor[2];
            matA(i, 3) = 1;
            vecB(i) = projectPos * nor;
        }
        matA(supportNum, 0) = mDir[0] * weight;
        matA(supportNum, 1) = mDir[1] * weight;
        matA(supportNum, 2) = mDir[2] * weight;
        matA(supportNum, 3) = 0;
        vecB(supportNum) = mDir * planePos * weight;

        Eigen::MatrixXd matAT = matA.transpose();
        Eigen::MatrixXd matCoefA = matAT * matA;
        Eigen::MatrixXd vecCoefB = matAT * vecB;
        Eigen::VectorXd res = matCoefA.ldlt().solve(vecCoefB);
        mCenter = Vector3(res(0), res(1), res(2));
        mRadius = fabs(res(3));*/
        if (mRadius > maxCylinderRadius)
        {
            mSupportVertex.clear();
            return 0;
        }
        //MagicLog << "Refit Cylinder: " << mDir[0] << " " << mDir[1] << " " << mDir[2] << " " << mRadius << " " 
        //    << mCenter[0] << " " << mCenter[1] << " " << mCenter[2] << std::endl;
        //Refit support vertex
        //Real MaxAngleDeviation = 0.94;
        //Real MaxDistDeviation = 0.001 * 10;
        Real MaxDistDeviation = mRadius * maxCylinderRadiusScale;
        //std::map<int, int> visitFlag;
        std::vector<bool> visitFlag(pMesh->GetVertexNumber(), 0);
        std::vector<int> searchIndex = mSupportVertex;
        for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
        {
            visitFlag[*itr] = 1;
        }
        mSupportVertex.clear();
        //mScore = 0;
        while (searchIndex.size() > 0)
        {
            std::vector<int> searchIndexNext;
            for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
            {
                //first check current vertex
                const Vertex3D* pVert = pMesh->GetVertex(*itr);
                Vector3 pos = pVert->GetPosition();
                Vector3 projectPos = pos + mDir * ((mCenter - pos) * mDir);
                Vector3 nor = pVert->GetNormal();
                Vector3 dir = projectPos - mCenter;
                Real length = dir.Normalise();
                Real angleDev = fabs(dir * nor);
                if (fabs(length - mRadius) > MaxDistDeviation || angleDev < maxAngleDeviation)
                {
                    continue;
                }
                //mScore += (angleDev - baseScore);
                mSupportVertex.push_back(*itr);
                //if current vertex pass, push its neighbors into searchIndexNext
                const Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    int newId = pEdge->GetVertex()->GetId();
                    if (visitFlag[newId] != 1)
                    {
                        visitFlag[newId] = 1;
                        if (resFlag.at(newId) == PrimitiveType::None)
                        {
                            searchIndexNext.push_back(newId);
                        }
                    }
                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }

        //MagicLog << "Refit Support vertex size: " << mSupportVertex.size() << std::endl;
        //MagicLog << "Cylinder refitting time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
        return mSupportVertex.size();
    }

    PrimitiveType CylinderCandidate::GetType()
    {
        return Cylinder;
    }

    void CylinderCandidate::UpdateScore(const Mesh3D* pMesh, std::vector<Real>& vertWeightList)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        mScore = 0;
        for (std::vector<int>::iterator itr = mSupportVertex.begin(); itr != mSupportVertex.end(); ++itr)
        {
            Vector3 pos = pMesh->GetVertex(*itr)->GetPosition();
            Vector3 projectPos = pos + mDir * ((mCenter - pos) * mDir);
            Vector3 nor = pMesh->GetVertex(*itr)->GetNormal();
            Vector3 dir = projectPos - mCenter;
            dir.Normalise();
            //mScore += (fabs(dir * nor) - baseScore) * vertWeightList.at(*itr);
            Real cosA = fabs(nor * dir);
            cosA = cosA > 1 ? 1 : cosA;
            /*Real scoreTrunc = baseScore - acos(cosA);
            scoreTrunc = scoreTrunc > 0 ? scoreTrunc : 0;
            mScore += scoreTrunc * vertWeightList.at(*itr);*/
            mScore += (baseScore - acos(cosA)) * vertWeightList.at(*itr);
        }
        //MagicLog << "Cylinder Update Score time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
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
        //calculate the intersection point of the three planes
        Vector3 nor0 = mpVert0->GetNormal();
        Vector3 nor1 = mpVert1->GetNormal();
        Vector3 interDir01 = nor0.CrossProduct(nor1);
        if (interDir01.Normalise() < Epsilon)
        {
            //MagicLog << "Parallel Plane" << std::endl;
            return false;
        }
        Vector3 lineDir0 = nor0.CrossProduct(interDir01);
        Vector3 pos0 = mpVert0->GetPosition();
        Vector3 pos1 = mpVert1->GetPosition();
        Vector3 interPos01 = pos0 + lineDir0 * ( (pos1 - pos0) * nor1 / (lineDir0 * nor1) );
        Vector3 pos2 = mpVert2->GetPosition();
        Vector3 nor2 = mpVert2->GetNormal();
        Real dotTemp = interDir01 * nor2;
        if (fabs(dotTemp) < Epsilon)
        {
            //MagicLog << "Parallel intersection line and plane" << std::endl;
            return false;
        }
        mApex = interPos01 + interDir01 * ( (pos2 - interPos01) * nor2 / dotTemp );
        //if (mApex.Length() > 1.5) //Here should be related with BBox.
        //{
        //    return false;
        //}
        Vector3 dir0 = pos0 - mApex;
        if (dir0.Normalise() < Epsilon)
        {
            //MagicLog << "Apex coincident" << std::endl;
            return false;
        }
        Vector3 dir1 = pos1 - mApex;
        if (dir1.Normalise() < Epsilon)
        {
            MagicLog << "cone: Apex coincident" << std::endl;
            return false;
        }
        Vector3 dir2 = pos2 - mApex;
        if (dir2.Normalise() < Epsilon)
        {
            MagicLog << "cone: Apex coincident" << std::endl;
            return false;
        }
        Vector3 planeDir0 = dir2 - dir0;
        Vector3 planeDir1 = dir2 - dir1;
        mDir = planeDir0.CrossProduct(planeDir1);
        if (mDir * dir0 < 0)
        {
            mDir *= -1;
        }
        if (mDir.Normalise() < Epsilon)
        {
            MagicLog << "cone: Cone Dir Len too Small" << std::endl;
            return false;
        }
        Real cos0 = mDir * dir0;
        cos0 = cos0 > 1 ? 1 : (cos0 < -1 ? -1 : cos0);
        Real cos1 = mDir * dir1;
        cos1 = cos1 > 1 ? 1 : (cos1 < -1 ? -1 : cos1);
        Real cos2 = mDir * dir2;
        cos2 = cos2 > 1 ? 1 : (cos2 < -1 ? -1 : cos2);
        mAngle = (acos(cos0) + acos(cos1) + acos(cos2)) / 3;
        if (mAngle > maxConeAngle || mAngle < minConeAngle)
        {
            MagicLog << "cone: Cone angle is too large: " << mAngle << std::endl;
            return false;
        }
        //MagicLog << "Cone: " << mApex[0] << " " << mApex[1] << " " << mApex[2] << " " << mAngle << " " << mDir[0] << " " << mDir[1] << " " << mDir[2] << std::endl;

        return true;
    }

    int ConeCandidate::CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        //Real MaxAngleDeviation = 0.1745329251994329; //10 degree
        //Real MaxCosAngleDeviation = 0.94;
        int id0 = mpVert0->GetId();
        int id1 = mpVert1->GetId();
        int id2 = mpVert2->GetId();
        mSupportVertex.clear();
        mSupportVertex.push_back(id0);
        mSupportVertex.push_back(id1);
        mSupportVertex.push_back(id2);
        //std::map<int, int> visitFlag;
        std::vector<bool> visitFlag(pMesh->GetVertexNumber(), 0);
        visitFlag[id0] = 1;
        visitFlag[id1] = 1;
        visitFlag[id2] = 1;
        std::vector<int> searchIndex;
        searchIndex.push_back(id0);
        searchIndex.push_back(id1);
        searchIndex.push_back(id2);
        while (searchIndex.size() > 0)
        {
            std::vector<int> searchIndexNext;
            for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
            {
                const Vertex3D* pVert = pMesh->GetVertex(*itr);
                const Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    const Vertex3D* pNewVert = pEdge->GetVertex();
                    pEdge = pEdge->GetPair()->GetNext();
                    int newId = pNewVert->GetId();
                    if (visitFlag[newId] != 1)
                    {
                        visitFlag[newId] = 1;
                        if (resFlag.at(newId) == PrimitiveType::None)
                        {
                            Vector3 pos = pNewVert->GetPosition();
                            Vector3 posDir = pos - mApex;
                            if (posDir.Normalise() <  Epsilon)
                            {
                                continue;
                            }
                            Real cosAngle = posDir * mDir;
                            cosAngle = cosAngle > 1 ? 1 : (cosAngle < -1 ? -1 : cosAngle);
                            Real angle = acos(cosAngle);
                            if (fabs(angle - mAngle) > maxConeAngleDeviation)
                            {
                                continue;
                            }
                            Vector3 dirTemp = mDir.CrossProduct(posDir);
                            if (dirTemp.Normalise() < Epsilon)
                            {
                                continue;
                            }
                            Vector3 ideaNor = dirTemp.CrossProduct(posDir);
                            ideaNor.Normalise();
                            Vector3 nor = pNewVert->GetNormal();
                            if (fabs(nor * ideaNor) < maxAngleDeviation)
                            {
                                continue;
                            }
                            searchIndexNext.push_back(newId);
                            mSupportVertex.push_back(newId);
                        } 
                    }
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }

        MagicLog << "cone: Support vertex size: " << mSupportVertex.size() << std::endl;
        //MagicLog << "Cone Support time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
        return mSupportVertex.size();
    }

    int ConeCandidate::Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag)
    {
        mHasRefit = true;
        //Refit parameter
        int supportNum = mSupportVertex.size();
        Eigen::MatrixXd matA(supportNum, 3);
        Eigen::VectorXd vecB(supportNum, 1);
        for (int i = 0; i < supportNum; i++)
        {
            const Vertex3D* pVert = pMesh->GetVertex( mSupportVertex.at(i) );
            Vector3 pos = pVert->GetPosition();
            Vector3 nor = pVert->GetNormal();
            matA(i, 0) = nor[0];
            matA(i, 1) = nor[1];
            matA(i, 2) = nor[2];
            vecB(i) = pos * nor;
        }
        Eigen::MatrixXd matAT = matA.transpose();
        Eigen::MatrixXd matCoefA = matAT * matA;
        Eigen::MatrixXd vecCoefB = matAT * vecB;
        Eigen::VectorXd res = matCoefA.ldlt().solve(vecCoefB);
        mApex = Vector3(res(0), res(1), res(2));
        //if (mApex.Length() > 1.5) //Should be related with BBox.
        //{
        //    mSupportVertex.clear();
        //    return 0;
        //}
        std::vector<Vector3> crossPosList;
        std::vector<Vector3> crossDirList;
        Vector3 avgCrossPos(0, 0, 0);
        for (std::vector<int>::iterator itr = mSupportVertex.begin(); itr != mSupportVertex.end(); ++itr)
        {
            Vector3 dir = pMesh->GetVertex(*itr)->GetPosition() - mApex;
            if (dir.Normalise() < Epsilon)
            {
                continue;
            }
            Vector3 pos = mApex + dir;
            avgCrossPos += pos;
            crossPosList.push_back(pos);
            crossDirList.push_back(dir);
        }
        if (crossPosList.size() == 0)
        {
            //MagicLog << "Cone Cross Position is Zero" << std::endl;
            mSupportVertex.clear();
            return 0;
        }
        int crossSize = mSupportVertex.size();
        avgCrossPos /= crossSize;
        std::vector<Vector3> crossDeltaPosList(crossSize);
        for (int i = 0; i < crossSize; i++)
        {
            crossDeltaPosList.at(i) = crossPosList.at(i) - avgCrossPos;
        }
        Eigen::Matrix3d mat;
        for (int xx = 0; xx < 3; xx++)
        {
            for (int yy = 0; yy < 3; yy++)
            {
                Real v = 0;
                for (int kk = 0; kk < crossSize; kk++)
                {
                    v += crossDeltaPosList[kk][xx] * crossDeltaPosList[kk][yy];
                }
                mat(xx, yy) = v;
            }
        }
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> es;
        es.compute(mat);
        Eigen::Vector3d dirVec = es.eigenvectors().col(0);
        mDir = Vector3(dirVec(0), dirVec(1), dirVec(2));
        //test whether it is a true cone
        //Real XX = 0;
        //Real YY = 0;
        //Real ZZ = 0;
        //Real XY = 0;
        //Real YZ = 0;
        //Real ZX = 0;
        //Real X = 0;
        //Real Y = 0;
        //Real Z = 0;
        //for (int i = 0; i < supportNum; i++)
        //{
        //    Vector3 nor = pMesh->GetVertex(mSupportVertex.at(i))->GetNormal();
        //    XX += nor[0] * nor[0];
        //    YY += nor[1] * nor[1];
        //    ZZ += nor[2] * nor[2];
        //    XY += nor[0] * nor[1];
        //    YZ += nor[1] * nor[2];
        //    ZX += nor[2] * nor[0];
        //    X += nor[0];
        //    Y += nor[1];
        //    Z += nor[2];
        //}
        //XX /= supportNum;
        //YY /= supportNum;
        //ZZ /= supportNum;
        //XY /= supportNum;
        //YZ /= supportNum;
        //ZX /= supportNum;
        //X /= supportNum;
        //Y /= supportNum;
        //Z /= supportNum;
        //Eigen::Matrix3d matVirify;
        //matVirify(0, 0) = XX - X * X;
        //matVirify(0, 1) = 2 * (XY - X * Y);
        //matVirify(0, 2) = 2 * (ZX - Z * X);
        //matVirify(1, 0) = 2 * (XY - X * Y);
        //matVirify(1, 1) = YY - Y * Y;
        //matVirify(1, 2) = 2 * (YZ - Y * Z);
        //matVirify(2, 0) = 2 * (ZX - Z * X);
        //matVirify(2, 1) = 2 * (YZ - Y * Z);
        //matVirify(2, 2) = ZZ - Z * Z;
        ////Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> es;
        //es.compute(matVirify);
        //Eigen::Vector3d dirVecVerify[3];
        //int rightEigenIndex = 0;
        //dirVecVerify[0] = es.eigenvectors().col(0);
        //Real eigenAngle = fabs(mDir[0] * dirVecVerify[0](0) + mDir[1] * dirVecVerify[0](1) + mDir[2] * dirVecVerify[0](2));
        //dirVecVerify[1] = es.eigenvectors().col(1);
        //Real eigenAngleTemp = fabs(mDir[0] * dirVecVerify[1](0) + mDir[1] * dirVecVerify[1](1) + mDir[2] * dirVecVerify[1](2));
        //if ( eigenAngleTemp > eigenAngle)
        //{
        //    rightEigenIndex = 1;
        //    eigenAngle = eigenAngleTemp;
        //}
        //dirVecVerify[2] = es.eigenvectors().col(2);
        //eigenAngleTemp = fabs(mDir[0] * dirVecVerify[2](0) + mDir[1] * dirVecVerify[2](1) + mDir[2] * dirVecVerify[2](2));
        //if (eigenAngleTemp > eigenAngle)
        //{
        //    rightEigenIndex = 2;
        //}
        //if (rightEigenIndex != 0)
        //{
        //    mSupportVertex.clear();
        //    return 0;
        //}
        //
        if (mDir.Normalise() < Epsilon)
        {
            //MagicLog << "Cone: mDir is Zero" << std::endl;
            mSupportVertex.clear();
            return 0;
        }
        if (mDir * crossDirList.at(0) < 0)
        {
            mDir *= -1;
        }
        Real angle = 0;
        for (int i = 0; i < crossSize; i++)
        {
            Real cos = mDir * crossDirList.at(i);
            cos = cos > 1 ? 1 : (cos < -1 ? -1 : cos);
            angle += acos(cos);
        }
        mAngle = angle / crossSize;
        if (mAngle > maxConeAngle || mAngle < minConeAngle)
        {
            mSupportVertex.clear();
            //MagicLog << "Cone refit angle is too large: " << mAngle << std::endl;
            return 0;
        }
        //MagicLog << "Refit Cone: " << mApex[0] << " " << mApex[1] << " " << mApex[2] << " " << mAngle << " " << mDir[0] << " " << mDir[1] << " " << mDir[2] << std::endl;

        //Refit support vertex
        //Real MaxAngleDeviation = 0.1745329251994329; //10 degree
        //Real MaxCosAngleDeviation = 0.94;
        //std::map<int, int> visitFlag;
        std::vector<bool> visitFlag(pMesh->GetVertexNumber(), 0);
        std::vector<int> searchIndex = mSupportVertex;
        for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
        {
            visitFlag[*itr] = 1;
        }
        mSupportVertex.clear();
        //mScore = 0;
        while (searchIndex.size() > 0)
        {
            std::vector<int> searchIndexNext;
            for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
            {
                //first check current vertex
                const Vertex3D* pVert = pMesh->GetVertex(*itr);
                Vector3 pos = pVert->GetPosition();
                Vector3 posDir = pos - mApex;
                if (posDir.Normalise() <  Epsilon)
                {
                    continue;
                }
                Real cosAngle = posDir * mDir;
                cosAngle = cosAngle > 1 ? 1 : (cosAngle < -1 ? -1 : cosAngle);
                Real angle = acos(cosAngle);
                if (fabs(angle - mAngle) > maxConeAngleDeviation)
                {
                    continue;
                }
                Vector3 dirTemp = mDir.CrossProduct(posDir);
                if (dirTemp.Normalise() < Epsilon)
                {
                    continue;
                }
                Vector3 ideaNor = dirTemp.CrossProduct(posDir);
                ideaNor.Normalise();
                Vector3 nor = pVert->GetNormal();
                Real angleDev = fabs(nor * ideaNor);
                if (angleDev < maxAngleDeviation)
                {
                    continue;
                }
                //mScore += (angleDev - baseScore);
                mSupportVertex.push_back(*itr);
                //if current vertex pass, push its neighbors into searchIndexNext
                const Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    int newId = pEdge->GetVertex()->GetId();
                    if (visitFlag[newId] != 1)
                    {
                        visitFlag[newId] = 1;
                        if (resFlag.at(newId) == PrimitiveType::None)
                        {
                            searchIndexNext.push_back(newId);
                        }
                    }
                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }
        MagicLog << "cone: Refitting support number: " << mSupportVertex.size() << std::endl;
        return mSupportVertex.size();
    }

    PrimitiveType ConeCandidate::GetType()
    {
        return Cone;
    }

    void ConeCandidate::UpdateScore(const Mesh3D* pMesh, std::vector<Real>& vertWeightList)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        mScore = 0;
        for (std::vector<int>::iterator itr = mSupportVertex.begin(); itr != mSupportVertex.end(); ++itr)
        {
            Vector3 pos = pMesh->GetVertex(*itr)->GetPosition();
            Vector3 posDir = pos - mApex;
            if (posDir.Normalise() < Epsilon)
            {
                continue;
            }
            Vector3 dirTemp = mDir.CrossProduct(posDir);
            if (dirTemp.Normalise() < Epsilon)
            {
                continue;
            }
            Vector3 ideaNor = dirTemp.CrossProduct(posDir);
            if (ideaNor.Normalise() < Epsilon)
            {
                continue;
            }
            Vector3 nor = pMesh->GetVertex(*itr)->GetNormal();
            //mScore += (fabs(nor * ideaNor) - baseScore) * vertWeightList.at(*itr);
            Real cosA = fabs(nor * ideaNor);
            cosA = cosA > 1 ? 1 : cosA;
            /*Real scoreTrunc = baseScore - acos(cosA);
            scoreTrunc = scoreTrunc > 0 ? scoreTrunc : 0;
            mScore += scoreTrunc * vertWeightList.at(*itr);*/
            mScore += (baseScore - acos(cosA)) * vertWeightList.at(*itr);
        }
        //MagicLog << "Cone Update Score time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl; 
    }

    PrimitiveDetection::PrimitiveDetection()
    {
    }

    PrimitiveDetection::~PrimitiveDetection()
    {
    }

    void PrimitiveDetection::Primitive2DSelection(Mesh3D* pMesh, std::vector<int>& res)
    {
        static std::vector<int> localRes;
        static Mesh3D* localMesh = NULL;
        static std::map<Real, ShapeCandidate* > candidateMap; 
        static std::vector<ShapeCandidate* > candidates;
        //Intialize flags
        /*int vertNum = pMesh->GetVertexNumber();
        res = std::vector<int>(vertNum, PrimitiveType::None);
        std::vector<int> featureMarks(vertNum, 0);
        CalFeatureBoundary(pMesh, featureMarks);*/
        int vertNum = pMesh->GetVertexNumber();
        if (localMesh != pMesh)
        {
            localMesh = pMesh;
            localRes = std::vector<int>(vertNum, PrimitiveType::None);
            for (int candId = 0; candId < candidates.size(); candId++)
            {
                if (candidates.at(candId) != NULL)
                {
                    delete candidates.at(candId);
                }
            }
            candidates.clear();
            candidateMap.clear();

            //Initialize Primitive Parameters
            pMesh->CalculateFaceArea();
            pMesh->CalculateBBox();
            Vector3 bboxMin, bboxMax;
            pMesh->GetBBox(bboxMin, bboxMax);
            Real bboxSize = (bboxMax - bboxMin).Length();
            maxDistDeviation = bboxSize * 0.004;
            maxSphereRadius = bboxSize / 2;
            maxCylinderRadius = bboxSize / 2;
        }
        res = localRes;
        std::vector<int> featureMarks(vertNum, 0);
        std::vector<Real> featureScores;
        CalFeatureScore(pMesh, featureMarks, featureScores);
        for (int vid = 0; vid < vertNum; vid++)
        {
            if (featureMarks.at(vid) == 1)
            {
                res.at(vid) = PrimitiveType::Other;
            }
        }
        std::vector<Real> vertWeightList;
        //pMesh->CalculateFaceArea();
        CalVertexWeight(pMesh, vertWeightList);

        //Get valid vertex
        std::map<Real, int> validMap;
        for (int i = 0; i < vertNum; i++)
        {
            if (featureMarks.at(i) == 0 && res.at(i) == PrimitiveType::None)
            {
                //validVert.push_back(i);
                validMap[featureScores.at(i)] = i;
            }
        }
        int validVertNum = validMap.size() / 5;
        MagicLog << "valid vertex number: " << validVertNum << std::endl;
        std::vector<int> validVert(validVertNum);
        int indexTemp = 0;
        for (std::map<Real, int>::iterator validItr = validMap.begin(); validItr != validMap.end(); ++validItr)
        {
            validVert.at(indexTemp) = validItr->second;
            indexTemp++;
            if (indexTemp == validVertNum)
            {
                break;
            }
        }
        //sample 
        int sampleNum = 10;
        std::vector<bool> sampleFlag(validVertNum, 0);
        sampleFlag.at(0) = true;
        std::vector<int> sampleIndex(sampleNum);
        sampleIndex.at(0) = 0;
        std::vector<Real> minDist(validVertNum, 1.0e10);
        int curIndex = 0;
        for (int sid = 1; sid < sampleNum; sid++)
        {
            Vector3 curPos = pMesh->GetVertex(validVert.at(curIndex))->GetPosition();
            Real maxDist = -1;
            int pos = -1;
            for (int vid = 0; vid < validVertNum; ++vid)
            {
                if (sampleFlag.at(vid) == 1)
                {
                    continue;
                }
                Real dist = (pMesh->GetVertex(validVert.at(vid))->GetPosition() - curPos).LengthSquared();
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
        //
        for (int sid = 0; sid < sampleNum; sid++)
        {
            int validSampleId = validVert.at(sampleIndex.at(sid));
            //Get vertex n neigbors
            int neighborRadius = 10;
            int minNeigborNum = 6;
            int neigborSampleNum = 5;
            std::vector<int> neighborList;
            std::vector<bool> visitFlag(pMesh->GetVertexNumber(), 0);
            std::vector<int> tranStack;
            tranStack.push_back(validSampleId);
            visitFlag[validSampleId] = 1;
            for (int k = 0; k < neighborRadius; k++)
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
                            if (featureMarks.at(newId) == 0 && res.at(newId) == PrimitiveType::None)
                            {
                                tranStackNext.push_back(newId);
                                neighborList.push_back(newId);
                            }
                        }
                        pEdgeNeig = pEdgeNeig->GetPair()->GetNext();
                    } while (pEdgeNeig != pVertNeig->GetEdge());
                }
                tranStack = tranStackNext;
            }
            int neighborSize = neighborList.size();
            MagicLog << "neighbor size: " << neighborSize << std::endl;

            Vertex3D* pVertCand0 = pMesh->GetVertex(validSampleId);
            int neighborSampleSize = neighborSize / 3;
            int neighborSampleIterSize = (neighborSampleSize > neigborSampleNum ? neigborSampleNum : neighborSampleSize);
            ShapeCandidate* bestCand = NULL;
            for (int neighborSampleIndex = 0; neighborSampleIndex < neighborSampleIterSize; neighborSampleIndex++)
            {
                const Vertex3D* pVertCand1 = pMesh->GetVertex( neighborList.at(neighborSize - 1 - neighborSampleIndex) );
                const Vertex3D* pVertCand2 = pMesh->GetVertex( neighborList.at(neighborSize - 1 - neighborSampleSize - neighborSampleIndex) );
                //res.at(neighborList.at(neighborSize - 1 - neighborSampleIndex)) = PrimitiveType::Blend;
                //res.at(neighborList.at(neighborSize - 1 - neighborSampleSize - neighborSampleIndex)) = PrimitiveType::Blend;
                //Add Plane Candidate
                ShapeCandidate* planeCand = new PlaneCandidate(pVertCand0, pVertCand1, pVertCand2);
                if (planeCand->IsValid())
                {
                    if (planeCand->CalSupportVertex(pMesh, res) > minInitSupportNum)
                    {
                        if (planeCand->Refitting(pMesh, res) > minInitSupportNum)
                        {
                            planeCand->UpdateScore(pMesh, vertWeightList);
                            planeCand->UpdateSupportArea(pMesh, vertWeightList);
                            MagicLog << "plane score: " << planeCand->GetScore() << std::endl;
                            if (bestCand == NULL)
                            {
                                bestCand = planeCand;
                            }
                            else if (bestCand->GetScore() < planeCand->GetScore())
                            {
                                delete bestCand;
                                bestCand = planeCand;
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
                ShapeCandidate* sphereCand = new SphereCandidate(pVertCand0, pVertCand1);
                if (sphereCand->IsValid())
                {
                    if (sphereCand->CalSupportVertex(pMesh, res) > minInitSupportNum)
                    {
                        if (sphereCand->Refitting(pMesh, res) > minInitSupportNum)
                        {
                            sphereCand->UpdateScore(pMesh, vertWeightList);
                            sphereCand->UpdateSupportArea(pMesh, vertWeightList);
                            MagicLog << "sphere score: " << sphereCand->GetScore() << std::endl;
                            if (bestCand == NULL)
                            {
                                bestCand = sphereCand;
                            }
                            else if (bestCand->GetScore() < sphereCand->GetScore())
                            {
                                delete bestCand;
                                bestCand = sphereCand;
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
                ShapeCandidate* cylinderCand = new CylinderCandidate(pVertCand0, pVertCand1);
                if (cylinderCand->IsValid())
                {
                    if (cylinderCand->CalSupportVertex(pMesh, res) > minInitSupportNum)
                    {
                        if (cylinderCand->Refitting(pMesh, res) > minInitSupportNum)
                        {
                            cylinderCand->UpdateScore(pMesh, vertWeightList);
                            cylinderCand->UpdateSupportArea(pMesh, vertWeightList);
                            MagicLog << "cylinder score: " << cylinderCand->GetScore() << std::endl;
                            if (bestCand == NULL)
                            {
                                bestCand = cylinderCand;
                            }
                            else if (bestCand->GetScore() < cylinderCand->GetScore())
                            {
                                delete bestCand;
                                bestCand = cylinderCand;
                            }
                            else
                            {
                                delete cylinderCand;
                            }
                        }
                        else
                        {
                            delete cylinderCand;
                        }
                    }
                    else
                    {
                        delete cylinderCand;
                    }
                }
                else
                {
                    delete cylinderCand;
                }
                //Add Cone Candidate
                ShapeCandidate* coneCand = new ConeCandidate(pVertCand0, pVertCand1, pVertCand2);
                if (coneCand->IsValid())
                {
                    if (coneCand->CalSupportVertex(pMesh, res) > minInitSupportNum)
                    {
                        if (coneCand->Refitting(pMesh, res) > minInitSupportNum)
                        {
                            coneCand->UpdateScore(pMesh, vertWeightList);
                            coneCand->UpdateSupportArea(pMesh, vertWeightList);
                            MagicLog << "cone score: " << coneCand->GetScore() << std::endl;
                            if (bestCand == NULL)
                            {
                                bestCand = coneCand;
                            }
                            else if (bestCand->GetScore() < coneCand->GetScore())
                            {
                                delete bestCand;
                                bestCand = coneCand;
                            }
                            else
                            {
                                delete coneCand;
                            }
                        }
                        else
                        {
                            delete coneCand;
                        }
                    }
                    else
                    {
                        delete coneCand;
                    }
                }
                else
                {
                    delete coneCand;
                }
            }
            if (bestCand != NULL)
            {
                MagicLog << "best score: " << bestCand->GetScore() << " area: " << bestCand->GetSupportArea() << std::endl;
                candidateMap[bestCand->GetScore()] = bestCand;
                //candidateMap[bestCand->GetSupportArea()] = bestCand;
                candidates.push_back(bestCand);
            }
        }
        
        int acceptNum = 3;
        int acceptId = 0;
        for (std::map<Real, ShapeCandidate* >::reverse_iterator candItr = candidateMap.rbegin(); candItr != candidateMap.rend(); ++candItr)
        {
            if (candItr->second->IsRemoved() == true)
            {
                continue;
            }
            if (acceptId == acceptNum)
            {
                break;
            }
            int candType = candItr->second->GetType();
            std::vector<int> supportVert = candItr->second->GetSupportVertex();
            for (std::vector<int>::iterator supportItr = supportVert.begin(); supportItr != supportVert.end(); ++supportItr)
            {
                res.at(*supportItr) = candType;
            }
            candItr->second->SetRemoved(true);
            RemoveAcceptableCandidate(candidates, res);
            for (int candId = 0; candId < candidates.size(); candId++)
            {
                if (candidates.at(candId)->IsRemoved())
                {
                    continue;
                }
                candidates.at(candId)->UpdateScore(pMesh, vertWeightList);
                candidates.at(candId)->UpdateSupportArea(pMesh, vertWeightList);
            }
            acceptId++;
        }
        for (int sid = 0; sid < sampleNum; sid++)
        {
            res.at(validVert.at(sampleIndex.at(sid))) = PrimitiveType::Blend;
        }
        
        localRes = res;
    }

    void PrimitiveDetection::Primitive2DDetection(Mesh3D* pMesh, std::vector<int>& res)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();

        //Intialize flags
        int vertNum = pMesh->GetVertexNumber();
        res = std::vector<int>(vertNum, PrimitiveType::None);
        std::vector<int> sampleFlag(vertNum, 0);
        CalFeatureBoundary(pMesh, sampleFlag);
        std::vector<Real> vertWeightList;
        pMesh->CalculateFaceArea();
        CalVertexWeight(pMesh, vertWeightList);

        //Initialize Primitive Parameters
        pMesh->CalculateBBox();
        Vector3 bboxMin, bboxMax;
        pMesh->GetBBox(bboxMin, bboxMax);
        Real bboxSize = (bboxMax - bboxMin).Length();
        maxDistDeviation = bboxSize * 0.004;
        maxSphereRadius = bboxSize / 2;
        maxCylinderRadius = bboxSize / 2;
        minSupportArea = 0;
        UpdateAcceptableArea(pMesh, res);

        //Initialize candidates   
        std::vector<ShapeCandidate* > candidates;
        MagicLog << "Mesh vertex number: " << pMesh->GetVertexNumber() << " face number: " << pMesh->GetFaceNumber() << std::endl;
        MagicLog << "prepare time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
        //Initialize iteration parameters
        int maxIterCount = 500;
        bool stopIteration = false;
        int lastBestPotential = -1;
        int noRefittedPotentialsTime = 0;

        for (int iterIndex = 0; iterIndex < maxIterCount; ++iterIndex)
        {
            if (AddNewCandidates(candidates, pMesh, res, sampleFlag, vertWeightList) == false)
            {
                MagicLog << "Stop: No New Candidates Found" << std::endl;
                break;
            }

            std::vector<int> potentials;
            ChosePotentials(candidates, potentials);
            //std::map<Real, int> bestSet;
            std::map<Real, int> refitedPotentials;
            int refitedNum = RefitPotentials(candidates, potentials, refitedPotentials, pMesh, res, vertWeightList);
            MagicLog << "Refited Potential number: " << refitedPotentials.size() << std::endl;
            if (refitedNum == 0)
            {
                noRefittedPotentialsTime++;
                if (noRefittedPotentialsTime == 10)
                {
                    MagicLog << "Stop: no refitted potentials" << std::endl;
                    break;
                }
            }
            else
            {
                noRefittedPotentialsTime = 0;
            }
            //Judge whether to accept 
            for (std::map<Real, int>::reverse_iterator itr = refitedPotentials.rbegin(); itr != refitedPotentials.rend(); ++itr)
            {
                MagicLog << "Candidate" << itr->second << " : " << candidates.at(itr->second)->GetSupportArea()
                         << "  mAcceptableArea: " << acceptableArea
                         << " mMinSupportArea: " << minSupportArea << std::endl;
                if (IsCandidateAcceptable(itr->second, candidates) || itr->second == lastBestPotential)
                {
                    //Mark Primitive Type
                    std::vector<int> supportVert = candidates.at(itr->second)->GetSupportVertex();
                    int candType = candidates.at(itr->second)->GetType();
                    for (std::vector<int>::iterator itr = supportVert.begin(); itr != supportVert.end(); ++itr)
                    {
                        res.at(*itr) = candType;
                    }

                    //Remove acceptable from candidates
                    candidates.at(itr->second)->SetRemoved(true);
                    RemoveAcceptableCandidate(candidates, res);
                    if (UpdateAcceptableArea(pMesh, res) == false)
                    {
                        stopIteration = true;
                        break;
                    }
                    //Update Primitive Score
                    for (int i = 0; i < candidates.size(); i++)
                    {
                        if (candidates.at(i)->IsRemoved())
                        {
                            continue;
                        }
                        candidates.at(i)->UpdateScore(pMesh, vertWeightList);
                        candidates.at(i)->UpdateSupportArea(pMesh, vertWeightList);
                    }
                    //
                }
                else
                {
                    lastBestPotential = itr->second;
                    break;
                }
            }
            if (stopIteration)
            {
                MagicLog << "Stop: Force Break" << std::endl;
                break;
            }
            
        }
        
        //Clear
        for (std::vector<ShapeCandidate* >::iterator itr = candidates.begin(); itr != candidates.end(); ++itr)
        {
            if (*itr != NULL)
            {
                delete *itr;
                *itr = NULL;
            }
        }
        candidates.clear();
        MagicLog << "PrimitiveDetection::Primitive2DDetection total time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
    }

    void PrimitiveDetection::CalVertexWeight(Mesh3D* pMesh, std::vector<Real>& vertWeightList)
    {
        //MagicLog << "PrimitiveDetection::CalVertexWeight" << std::endl;
        vertWeightList.clear();
        int vertNum = pMesh->GetVertexNumber();
        vertWeightList.resize(vertNum);
        for (int i = 0; i < vertNum; i++)
        {
            Vertex3D* pVert = pMesh->GetVertex(i);
            Edge3D* pEdge = pVert->GetEdge();
            Real area = 0;
            do
            {
                Face3D* pFace = pEdge->GetFace();
                if (pFace != NULL)
                {
                    area += pFace->GetArea();
                }
                pEdge = pEdge->GetPair()->GetNext();
            } while (pEdge != NULL && pEdge != pVert->GetEdge());
            vertWeightList.at(i) = area;
        }
    }

    bool PrimitiveDetection::AddNewCandidates(std::vector<ShapeCandidate* >& candidates, const Mesh3D* pMesh, 
            std::vector<int>& res, std::vector<int>& sampleFlag, std::vector<Real>& vertWeightList)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        std::vector<int> validVert;
        for (int i = 0; i < res.size(); i++)
        {
            if (res.at(i) == PrimitiveType::None && sampleFlag.at(i) == 0)
            {
                validVert.push_back(i);
            }
        }
        int validVertNum = validVert.size();
        MagicLog << "Valid vertex number: " << validVertNum << std::endl;
        if (validVertNum < 500)
        {
            return false;
        }

        int sampleNum = 10;
        int sampleDelta = validVertNum / sampleNum;
        int neighborRadius = 10;
        int minNeigborNum = 6;
        int neigborSampleNum = 5;
        bool isNewAdded = false;
        int sampledNumber = 0;
        int skipNumber = 0;
        int sampleIndex = 0;
        //for (int sampleIndex = 0; sampleIndex < validVertNum; sampleIndex += sampleDelta)
        while (sampledNumber < sampleNum && skipNumber < sampleNum)
        {
            sampleIndex = (sampleIndex + sampleDelta) % validVertNum;
            float iterTime = MagicCore::ToolKit::GetSingleton()->GetTime();
            acceptableArea -= acceptableAreaDelta;
            if (acceptableArea < minSupportArea)
            {
                acceptableArea = minSupportArea;
                MagicLog << "Stop: acceptableArea < minSupportArea" << std::endl;
                return false;
            }
            int currentIndex = validVert.at(sampleIndex);
            if (sampleFlag.at(currentIndex) == 1)
            {
                skipNumber++;
                continue;
            }
            sampleFlag.at(currentIndex) = 1;
            const Vertex3D* pVert = pMesh->GetVertex(currentIndex);
            //Get vertex n neigbors
            std::vector<int> neighborList;
            std::vector<bool> visitFlag(pMesh->GetVertexNumber(), 0);
            std::vector<int> tranStack;
            tranStack.push_back(currentIndex);
            visitFlag[currentIndex] = 1;
            for (int k = 0; k < neighborRadius; k++)
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
                            if (res.at(newId) == PrimitiveType::None && sampleFlag.at(newId) == 0)
                            {
                                tranStackNext.push_back(newId);
                                //if (k > 0)
                                {
                                    neighborList.push_back(newId);
                                }
                            }
                        }
                        pEdgeNeig = pEdgeNeig->GetPair()->GetNext();
                    } while (pEdgeNeig != pVertNeig->GetEdge());
                }
                tranStack = tranStackNext;
            }
            int neighborSize = neighborList.size();
            if (neighborSize < minNeigborNum)
            {
                MagicLog << "unluck: neighborSize too small: " << neighborSize << std::endl;
                skipNumber++;
                continue;
            }
            else
            {
                skipNumber = 0;
                sampledNumber++;
            }
            const Vertex3D* pVertCand0 = pVert;
            int neighborSampleSize = neighborSize / 3;
            int neighborSampleIterSize = (neighborSampleSize > neigborSampleNum ? neigborSampleNum : neighborSampleSize);
            ShapeCandidate* bestCand = NULL;
            for (int neighborSampleIndex = 0; neighborSampleIndex < neighborSampleIterSize; neighborSampleIndex++)
            {
                const Vertex3D* pVertCand1 = pMesh->GetVertex( neighborList.at(neighborSize - 1 - neighborSampleIndex) );
                const Vertex3D* pVertCand2 = pMesh->GetVertex( neighborList.at(neighborSize - 1 - neighborSampleSize - neighborSampleIndex) );
                //Add Plane Candidate
                ShapeCandidate* planeCand = new PlaneCandidate(pVertCand0, pVertCand1, pVertCand2);
                if (planeCand->IsValid())
                {
                    if (planeCand->CalSupportVertex(pMesh, res) > minInitSupportNum)
                    {
                        planeCand->UpdateScore(pMesh, vertWeightList);
                        planeCand->UpdateSupportArea(pMesh, vertWeightList);
                        if (bestCand == NULL)
                        {
                            bestCand = planeCand;
                        }
                        else if (bestCand->GetScore() < planeCand->GetScore())
                        {
                            delete bestCand;
                            bestCand = planeCand;
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
                }
                else
                {
                    delete planeCand;
                }
                //Add Sphere Candidate
                ShapeCandidate* sphereCand = new SphereCandidate(pVertCand0, pVertCand1);
                if (sphereCand->IsValid())
                {
                    if (sphereCand->CalSupportVertex(pMesh, res) > minInitSupportNum)
                    {
                        sphereCand->UpdateScore(pMesh, vertWeightList);
                        sphereCand->UpdateSupportArea(pMesh, vertWeightList);
                        if (bestCand == NULL)
                        {
                            bestCand = sphereCand;
                        }
                        else if (bestCand->GetScore() < sphereCand->GetScore())
                        {
                            delete bestCand;
                            bestCand = sphereCand;
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
                }
                else
                {
                    delete sphereCand;
                }
                //Add Cylinder Candidate
                ShapeCandidate* cylinderCand = new CylinderCandidate(pVertCand0, pVertCand1);
                if (cylinderCand->IsValid())
                {
                    if (cylinderCand->CalSupportVertex(pMesh, res) > minInitSupportNum)
                    {
                        cylinderCand->UpdateScore(pMesh, vertWeightList);
                        cylinderCand->UpdateSupportArea(pMesh, vertWeightList);
                        if (bestCand == NULL)
                        {
                            bestCand = cylinderCand;
                        }
                        else if (bestCand->GetScore() < cylinderCand->GetScore())
                        {
                            delete bestCand;
                            bestCand = cylinderCand;
                        }
                        else
                        {
                            delete cylinderCand;
                        }
                    }
                    else
                    {
                        delete cylinderCand;
                    }
                }
                else
                {
                    delete cylinderCand;
                }
                //Add Cone Candidate
                ShapeCandidate* coneCand = new ConeCandidate(pVertCand0, pVertCand1, pVertCand2);
                if (coneCand->IsValid())
                {
                    if (coneCand->CalSupportVertex(pMesh, res) > minInitSupportNum)
                    {
                        coneCand->UpdateScore(pMesh, vertWeightList);
                        coneCand->UpdateSupportArea(pMesh, vertWeightList);
                        if (bestCand == NULL)
                        {
                            bestCand = coneCand;
                        }
                        else if (bestCand->GetScore() < coneCand->GetScore())
                        {
                            delete bestCand;
                            bestCand = coneCand;
                        }
                        else
                        {
                            delete coneCand;
                        }
                    }
                    else
                    {
                        delete coneCand;
                    }
                }
                else
                {
                    delete coneCand;
                }
                //check luck break;
                if (bestCand != NULL)
                {
                    if (bestCand->GetSupportArea() > acceptableArea)
                    {
                        MagicLog << "Super Luck break in FindNewCandidates" << std::endl;
                        candidates.push_back(bestCand);
                        return true;
                    }
                }
            }
            if (bestCand != NULL)
            {
                candidates.push_back(bestCand);
                isNewAdded = true;
                if (bestCand->GetSupportArea() > acceptableArea)
                {
                    MagicLog << "Luck break in FindNewCandidates" << std::endl;
                    break;
                }
            }
        }

        if (isNewAdded == false)
        {
            MagicLog << "Stop: No new candidates added" << std::endl;
        }
        //MagicLog << "   FindNewCandidates: " << candidates.size() << std::endl;
        //MagicLog << "FindNewCandidates time: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;;
        return isNewAdded;
    }

    bool PrimitiveDetection::IsCandidateAcceptable(int index, std::vector<ShapeCandidate* >& candidates)
    {
        if (candidates.at(index)->GetSupportArea() > acceptableArea)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void PrimitiveDetection::RemoveAcceptableCandidate(std::vector<ShapeCandidate* >& candidates, const std::vector<int>& res)
    {
        int candNum = candidates.size();
        for (int i = 0; i < candNum; i++)
        {
            ShapeCandidate* pCand = candidates.at(i);
            if (pCand->IsRemoved())
            {
                continue;
            }
            std::vector<int> newSupportVertex;
            std::vector<int> oldSupportVertex = pCand->GetSupportVertex();
            int supportNum = oldSupportVertex.size();
            for (int j = 0; j < supportNum; j++)
            {
                if (res.at(oldSupportVertex.at(j)) == PrimitiveType::None)
                {
                    newSupportVertex.push_back(oldSupportVertex.at(j));
                }
            }
            if (newSupportVertex.size() < minSupportNum)
            {
                newSupportVertex.clear();
                pCand->SetRemoved(true);
            }
            pCand->SetSupportVertex(newSupportVertex);
        }
    }

    void PrimitiveDetection::ChosePotentials(std::vector<ShapeCandidate* >& candidates, std::vector<int>& potentials)
    {
        std::map<Real, int> scoreMap;
        int candNum = candidates.size();
        for (int candid = 0; candid < candNum; candid++)
        {
            if (candidates.at(candid)->IsRemoved())
            {
                continue;
            }
            else
            {
                scoreMap[candidates.at(candid)->GetScore()] = candid;
            }
        }
        int validCandNum = scoreMap.size();
        int bestNum = 3;
        if (validCandNum < bestNum)
        {
            bestNum = validCandNum;
        }
        int bestIdx = 0;
        potentials.clear();
        potentials.resize(bestNum);
        for (std::map<Real, int>::reverse_iterator itr = scoreMap.rbegin(); itr != scoreMap.rend(); ++itr)
        {
            if (bestIdx >= bestNum)
            {
                break;
            }
            potentials.at(bestIdx) = itr->second;
            bestIdx++;
        }
        MagicLog << "validCandNum: " << validCandNum << " potentialNum: " << bestNum << std::endl;
    }

    bool PrimitiveDetection::UpdateAcceptableArea(Mesh3D* pMesh, std::vector<int>& res)
    {
        Real validArea = 0;
        int faceNum = pMesh->GetFaceNumber();
        for (int fid = 0; fid < faceNum; fid++)
        {
            Edge3D* pEdge = pMesh->GetFace(fid)->GetEdge();
            if (res.at(pEdge->GetVertex()->GetId()) == PrimitiveType::None)
            {
                validArea += pMesh->GetFace(fid)->GetArea();
                continue;
            }
            if (res.at(pEdge->GetNext()->GetVertex()->GetId()) == PrimitiveType::None)
            {
                validArea += pMesh->GetFace(fid)->GetArea();
                continue;
            }
            if (res.at(pEdge->GetPre()->GetVertex()->GetId()) == PrimitiveType::None)
            {
                validArea += pMesh->GetFace(fid)->GetArea();
                continue;
            }
        }
        acceptableArea = validArea * acceptableAreaScale;
        acceptableAreaDelta = acceptableArea / 500;
        minSupportArea = validArea / 100;
        MagicLog << "UpdateAcceptableArea: valid: " << validArea << " Acceptable: " << acceptableArea << std::endl;
        return true;
    }

    void PrimitiveDetection::CalFeatureBoundary(Mesh3D* pMesh, std::vector<int>& features)
    {
        int vertNum = pMesh->GetVertexNumber();
        Real scale = 5;
        if (vertNum > 100000)
        {
            scale = 6;
        }
        if (vertNum > 500000)
        {
            scale = 7;
        }
        if (vertNum > 1000000)
        {
            scale = 8;
        }
        for (int vid = 0; vid < vertNum; vid++)
        {
            std::vector<int> neighborList;
            neighborList.reserve(10);
            MagicDGP::Vertex3D* pVert = pMesh->GetVertex(vid);
            MagicDGP::Edge3D* pEdge = pVert->GetEdge();
            do
            {
                if (pEdge == NULL)
                {
                    break;
                }
                neighborList.push_back(pEdge->GetVertex()->GetId());
                pEdge = pEdge->GetPair()->GetNext();
            } while (pEdge != pVert->GetEdge());

            MagicDGP::Vector3 normal = pMesh->GetVertex(vid)->GetNormal();
            MagicDGP::Real nDev = 0;
            for (std::vector<int>::iterator neigItr = neighborList.begin(); neigItr != neighborList.end(); ++neigItr)
            {
                nDev += (normal - (pMesh->GetVertex(*neigItr)->GetNormal())).Length();
            }
            if (neighborList.size() > 0)
            {
                nDev /= neighborList.size();
            }
            nDev = nDev * scale + 0.2;
            if (nDev > 1)
            {
                //features.at(vid) = PrimitiveType::Blend;
                features.at(vid) = 1;
            }
        }
    }

    void PrimitiveDetection::CalFeatureScore(Mesh3D* pMesh, std::vector<int>& features, std::vector<Real>& scores)
    {
        int vertNum = pMesh->GetVertexNumber();
        Real scale = 5;
        //if (vertNum > 100000)
        //{
        //    scale = 6;
        //}
        //if (vertNum > 500000)
        //{
        //    scale = 7;
        //}
        //if (vertNum > 1000000)
        //{
        //    scale = 8;
        //}
        std::vector<MagicDGP::Real> norDev(vertNum);
        for (int vid = 0; vid < vertNum; vid++)
        {
            std::vector<int> neighborList;
            neighborList.reserve(10);
            MagicDGP::Vertex3D* pVert = pMesh->GetVertex(vid);
            MagicDGP::Edge3D* pEdge = pVert->GetEdge();
            do
            {
                if (pEdge == NULL)
                {
                    break;
                }
                neighborList.push_back(pEdge->GetVertex()->GetId());
                pEdge = pEdge->GetPair()->GetNext();
            } while (pEdge != pVert->GetEdge());

            MagicDGP::Vector3 normal = pMesh->GetVertex(vid)->GetNormal();
            MagicDGP::Real nDev = 0;
            for (std::vector<int>::iterator neigItr = neighborList.begin(); neigItr != neighborList.end(); ++neigItr)
            {
                //nDev += (normal - (pMesh->GetVertex(*neigItr)->GetNormal())).Length();
                Real cosA = normal * (pMesh->GetVertex(*neigItr)->GetNormal());
                cosA = cosA > 1 ? 1 : (cosA < -1 ? -1 : cosA);
                nDev += acos(cosA);
            }
            if (neighborList.size() > 0)
            {
                nDev /= neighborList.size();
            }
            norDev.at(vid) = nDev;
        }
        scores.clear();
        scores.resize(vertNum);
        for (int vid = 0; vid < vertNum; vid++)
        {
            MagicDGP::Vertex3D* pVert = pMesh->GetVertex(vid);
            MagicDGP::Edge3D* pEdge = pVert->GetEdge();
            MagicDGP::Real devGrad = 0;
            int neigNum = 0;
            do
            {
                if (pEdge == NULL)
                {
                    break;
                }
                devGrad += fabs(norDev.at(vid) - norDev.at(pEdge->GetVertex()->GetId()));
                neigNum++;
                pEdge = pEdge->GetPair()->GetNext();
            } while (pEdge != pVert->GetEdge());
            if (neigNum > 0)
            {
                devGrad /= neigNum;
            }
            scores.at(vid) = devGrad;
            devGrad = devGrad * scale + 0.2;
            if (devGrad > 0.5)
            {
                features.at(vid) = 1;
            }
        }
    }

    int PrimitiveDetection::RefitPotentials(std::vector<ShapeCandidate* >& candidates, std::vector<int>& potentials, std::map<Real, int>& refitedPotentials,
            Mesh3D* pMesh, std::vector<int>& resFlag, std::vector<Real>& vertWeightList)
    {
        refitedPotentials.clear();
        for (std::vector<int>::iterator itr = potentials.begin(); itr != potentials.end(); ++itr)
        {
            if (candidates.at(*itr)->HasRefit())
            {
                if (candidates.at(*itr)->GetSupportArea() > minSupportArea)
                {
                    refitedPotentials[candidates.at(*itr)->GetScore()] = *itr;
                    if (candidates.at(*itr)->GetSupportArea() > acceptableArea)
                    {
                        MagicLog << "Luck break" << std::endl;
                        return refitedPotentials.size();
                    }
                }
            }
            else
            {
                if (candidates.at(*itr)->Refitting(pMesh, resFlag) > minSupportNum)
                {
                    candidates.at(*itr)->UpdateSupportArea(pMesh, vertWeightList);
                    if (candidates.at(*itr)->GetSupportArea() > minSupportArea)
                    {
                        candidates.at(*itr)->UpdateScore(pMesh, vertWeightList);
                        refitedPotentials[candidates.at(*itr)->GetScore()] = *itr;
                        if (candidates.at(*itr)->GetSupportArea() > acceptableArea)
                        {
                            MagicLog << "Luck break" << std::endl;
                            return refitedPotentials.size();
                        }
                    }
                    else
                    {
                        candidates.at(*itr)->SetRemoved(true);
                    }
                }
                else
                {
                    candidates.at(*itr)->SetRemoved(true);
                }
            }
        }
        return refitedPotentials.size();
    }
}
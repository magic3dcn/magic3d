#include "PrimitiveDetection.h"
#include "Eigen/Dense"

namespace MagicDGP
{
    Real PrimitiveParameters::mMaxSphereRadius = 10;
    Real PrimitiveParameters::mMaxCylinderRadius = 10;

    PrimitiveParameters::PrimitiveParameters()
    {
    }

    PrimitiveParameters::~PrimitiveParameters()
    {
    }

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
        //Real MaxAngleDeviation = 0.9848;
        Real MaxAngleDeviation = 0.94;
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

    int PlaneCandidate::Refitting(const Mesh3D* pMesh)
    {
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
        Real MaxAngleDeviation = 0.94;
        Real MaxDistDeviation = 0.001 * 10;
        std::map<int, int> visitFlag;
        std::vector<int> searchIndex = mSupportVertex;
        for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
        {
            visitFlag[*itr] = 1;
        }
        mSupportVertex.clear();
        while (searchIndex.size() > 0)
        {
            std::vector<int> searchIndexNext;
            for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
            {
                //first check current vertex
                const Vertex3D* pVert = pMesh->GetVertex(*itr);
                Vector3 pos = pVert->GetPosition();
                Real distDev = fabs( (pos - mCenter) * mNormal );
                if (distDev > MaxDistDeviation)
                {
                    continue;
                }
                Vector3 nor = pVert->GetNormal();
                Real angleDev = fabs( nor * mNormal );
                if (angleDev < MaxAngleDeviation)
                {
                    continue;
                }
                mSupportVertex.push_back(*itr);
                //if current vertex pass, push its neighbors into searchIndexNext
                const Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    int newId = pEdge->GetVertex()->GetId();
                    if (visitFlag[newId] != 1)
                    {
                        visitFlag[newId] = 1;
                        searchIndexNext.push_back(newId);
                    }
                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }

        MagicLog << "Refit Support vertex size: " << mSupportVertex.size() << std::endl;

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
        if (mRadius > PrimitiveParameters::mMaxSphereRadius)
        {
            MagicLog << "Sphere radius is too large: " << mRadius << std::endl;
            return false;
        }
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
        //Real MaxAngleDeviation = 0.9848;
        Real MaxAngleDeviation = 0.94;
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

    int SphereCandidate::Refitting(const Mesh3D* pMesh)
    {
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
        mRadius = res(3);
        MagicLog << "Refit sphere: " << mCenter[0] << " " << mCenter[1] << " " << mCenter[2] << " " << mRadius << std::endl;
        //Refit support vertex
        Real MaxAngleDeviation = 0.94;
        //Real MaxDistDeviation = 0.001 * 10;
        Real MaxDistDeviation = mRadius * 0.05;
        std::map<int, int> visitFlag;
        std::vector<int> searchIndex = mSupportVertex;
        for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
        {
            visitFlag[*itr] = 1;
        }
        mSupportVertex.clear();
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
                if (fabs(length - mRadius) > MaxDistDeviation || fabs(dir * nor) < MaxAngleDeviation)
                {
                    continue;
                }
                mSupportVertex.push_back(*itr);
                //if current vertex pass, push its neighbors into searchIndexNext
                const Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    int newId = pEdge->GetVertex()->GetId();
                    if (visitFlag[newId] != 1)
                    {
                        visitFlag[newId] = 1;
                        searchIndexNext.push_back(newId);
                    }
                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }

        MagicLog << "Refit Support vertex size: " << mSupportVertex.size() << std::endl;

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
        Real MaxAngleDeviation = 0.9848;
        Real MaxDistDeviation = 0.001;
        Vector3 nor0 = mpVert0->GetNormal();
        Vector3 nor1 = mpVert1->GetNormal();
        mDir = nor0.CrossProduct(nor1);
        Real dirLen = mDir.Normalise();
        if (dirLen < Epsilon)
        {
            MagicLog << "dirLen is too small: " << dirLen << std::endl;
            return false;
        }
        Vector3 dirX = nor0;
        Vector3 dirY = mDir.CrossProduct(dirX);
        dirY.Normalise();
        Real nor1ProjectX = dirX * nor1;
        Real nor1ProjectY = dirY * nor1;
        if (fabs(nor1ProjectY) < Epsilon)
        {
            MagicLog << "nor1ProjectY is too small: " << nor1ProjectY << std::endl;
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
        if (fabs(radius2 - mRadius) > MaxDistDeviation)
        {
            MagicLog << "Radius are too different: " << radius2 - mRadius << std::endl;
            return false;
        }
        mRadius = (mRadius + radius2) / 2;
        if (mRadius > PrimitiveParameters::mMaxCylinderRadius)
        {
            MagicLog << "Cylinder radius is too large: " << mRadius << std::endl;
            return false;
        }
        MagicLog << "Cylinder is Valid: " << mDir[0] << " " << mDir[1] << " " << mDir[2] << " " << mRadius << " " 
            << mCenter[0] << " " << mCenter[1] << " " << mCenter[2] << std::endl;
        //reject condition: No condition

        return true;
    }

    int CylinderCandidate::CalSupportVertex(const Mesh3D* pMesh)
    {
        //Real MaxAngleDeviation = 0.9848;
        //Real MaxDistDeviation = 0.001;
        Real MaxAngleDeviation = 0.94;
        Real MaxDistDeviation = mRadius * 0.05;
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

    int CylinderCandidate::Refitting(const Mesh3D* pMesh)
    {
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
        MagicLog << "Eigne Value: " << std::endl << es.eigenvalues() << std::endl;
        MagicLog << "Eigen Vector: " << std::endl << es.eigenvectors() << std::endl;
        mDir = Vector3(dirVec[rightEigenIndex](0), dirVec[rightEigenIndex](1), dirVec[rightEigenIndex](2));
        mDir.Normalise();
        Vector3 planePos = pMesh->GetVertex(mSupportVertex.at(0))->GetPosition();
        Eigen::MatrixXd matA(supportNum, 4);
        Eigen::VectorXd vecB(supportNum, 1);
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
        Eigen::MatrixXd matAT = matA.transpose();
        Eigen::MatrixXd matCoefA = matAT * matA;
        Eigen::MatrixXd vecCoefB = matAT * vecB;
        Eigen::VectorXd res = matCoefA.ldlt().solve(vecCoefB);
        mCenter = Vector3(res(0), res(1), res(2));
        mRadius = res(3);
        MagicLog << "Refit Cylinder: " << mDir[0] << " " << mDir[1] << " " << mDir[2] << " " << mRadius << " " 
            << mCenter[0] << " " << mCenter[1] << " " << mCenter[2] << std::endl;
        //Refit support vertex
        Real MaxAngleDeviation = 0.94;
        //Real MaxDistDeviation = 0.001 * 10;
        Real MaxDistDeviation = mRadius * 0.05;
        std::map<int, int> visitFlag;
        std::vector<int> searchIndex = mSupportVertex;
        for (std::vector<int>::iterator itr = searchIndex.begin(); itr != searchIndex.end(); ++itr)
        {
            visitFlag[*itr] = 1;
        }
        mSupportVertex.clear();
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
                if (fabs(length - mRadius) > MaxDistDeviation || fabs(dir * nor) < MaxAngleDeviation)
                {
                    continue;
                }
                mSupportVertex.push_back(*itr);
                //if current vertex pass, push its neighbors into searchIndexNext
                const Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    int newId = pEdge->GetVertex()->GetId();
                    if (visitFlag[newId] != 1)
                    {
                        visitFlag[newId] = 1;
                        searchIndexNext.push_back(newId);
                    }
                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }

        MagicLog << "Refit Support vertex size: " << mSupportVertex.size() << std::endl;

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
        //calculate the intersection point of the three planes
        Vector3 nor0 = mpVert0->GetNormal();
        Vector3 nor1 = mpVert1->GetNormal();
        Vector3 interDir01 = nor0.CrossProduct(nor1);
        if (interDir01.Normalise() < Epsilon)
        {
            MagicLog << "Parallel Plane" << std::endl;
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
            MagicLog << "Parallel intersection line and plane" << std::endl;
            return false;
        }
        mApex = interPos01 + interDir01 * ( (pos2 - interPos01) * nor2 / dotTemp );
        Vector3 dir0 = pos0 - mApex;
        if (dir0.Normalise() < Epsilon)
        {
            MagicLog << "Apex coincident" << std::endl;
            return false;
        }
        Vector3 dir1 = pos1 - mApex;
        if (dir1.Normalise() < Epsilon)
        {
            MagicLog << "Apex coincident" << std::endl;
            return false;
        }
        Vector3 dir2 = pos2 - mApex;
        if (dir2.Normalise() < Epsilon)
        {
            MagicLog << "Apex coincident" << std::endl;
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
            MagicLog << "Cone Dir Len too Small" << std::endl;
            return false;
        }
        Real cos0 = mDir * dir0;
        cos0 = cos0 > 1 ? 1 : (cos0 < -1 ? -1 : cos0);
        Real cos1 = mDir * dir1;
        cos1 = cos1 > 1 ? 1 : (cos1 < -1 ? -1 : cos1);
        Real cos2 = mDir * dir2;
        cos2 = cos2 > 1 ? 1 : (cos2 < -1 ? -1 : cos2);
        mAngle = (acos(cos0) + acos(cos1) + acos(cos2)) / 3;

        return true;
    }

    int ConeCandidate::CalSupportVertex(const Mesh3D* pMesh)
    {
        Real MaxAngleDeviation = 0.1745329251994329; //10 degree
        Real MaxCosAngleDeviation = 0.9848;
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
                        Vector3 pos = pNewVert->GetPosition();
                        Vector3 posDir = pos - mApex;
                        if (posDir.Normalise() <  Epsilon)
                        {
                            continue;
                        }
                        Real cosAngle = posDir * mDir;
                        cosAngle = cosAngle > 1 ? 1 : (cosAngle < -1 ? -1 : cosAngle);
                        Real angle = acos(cosAngle);
                        if (fabs(angle - mAngle) > MaxAngleDeviation)
                        {
                            continue;
                        }
                        Vector3 dirTemp = mDir.CrossProduct(posDir);
                        if (dirTemp.Normalise() < Epsilon)
                        {
                            continue;
                        }
                        Vector3 ideaNor = dirTemp.CrossProduct(posDir);
                        Vector3 nor = pNewVert->GetNormal();
                        if (nor * ideaNor < MaxCosAngleDeviation)
                        {
                            continue;
                        }
                        searchIndexNext.push_back(newId);
                        mSupportVertex.push_back(newId);
                    }
                } while (pEdge != pVert->GetEdge() && pEdge != NULL);
            }
            searchIndex = searchIndexNext;
        }

        MagicLog << "Support vertex size: " << mSupportVertex.size() << std::endl;

        return mSupportVertex.size();
    }

    int ConeCandidate::Refitting(const Mesh3D* pMesh)
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
        res = std::vector<int>(vertNum, 6);
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
                    planeCand->Refitting(pMesh);
                    std::vector<int> supportList = planeCand->GetSupportVertex();
                    for (int i = 0; i < supportList.size(); i++)
                    {
                        res.at(supportList.at(i)) = 1;
                    }
                    res.at(pVert->GetId()) = 2;
                    res.at(pVertNeig0->GetId()) = 2;
                    res.at(pVertNeig1->GetId()) = 2;
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
            /*ShapeCandidate* sphereCand = new SphereCandidate(pVert, pVertNeig1);
            if (sphereCand->IsValid())
            {
                if (sphereCand->CalSupportVertex(pMesh) > minSupportNum)
                {
                    sphereCand->Refitting(pMesh);
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
            }*/
            //
            //Add Cylinder Candidate
            ShapeCandidate* cylinderCand = new CylinderCandidate(pVert, pVertNeig1);
            if (cylinderCand->IsValid())
            {
                if (cylinderCand->CalSupportVertex(pMesh) > minSupportNum)
                {
                    cylinderCand->Refitting(pMesh);
                    std::vector<int> supportList = cylinderCand->GetSupportVertex();
                    for (int i = 0; i < supportList.size(); i++)
                    {
                        res.at(supportList.at(i)) = 4;
                    }
                    res.at(pVert->GetId()) = 2;
                    res.at(pVertNeig1->GetId()) = 2;
                    break;
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
            //
            //Add Cone Candidate
            //ShapeCandidate* coneCand = new ConeCandidate(pVert, pVertNeig0, pVertNeig1);
            //if (coneCand->IsValid())
            //{
            //    //if (sphereCand->CalSupportVertex(pMesh) > minSupportNum)
            //    if (coneCand->CalSupportVertex(pMesh) > 100)
            //    {
            //        std::vector<int> supportList = coneCand->GetSupportVertex();
            //        for (int i = 0; i < supportList.size(); i++)
            //        {
            //            res.at(supportList.at(i)) = 5;
            //        }
            //        res.at(pVert->GetId()) = 2;
            //        res.at(pVertNeig0->GetId()) = 2;
            //        res.at(pVertNeig1->GetId()) = 2;
            //        break;
            //    }
            //    else
            //    {
            //        delete coneCand;
            //    }
            //}
            //else
            //{
            //    delete coneCand;
            //}
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
        return 0;
    }

    bool PrimitiveDetection::IsCandidateAcceptable(int index, std::vector<ShapeCandidate* >& candidates)
    {
        return true;
    }

    void PrimitiveDetection::RemoveAcceptableCandidate(int index, std::vector<ShapeCandidate* >& candidates)
    {

    }
}
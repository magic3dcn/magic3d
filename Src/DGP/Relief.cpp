#include "Relief.h"
#include <set>
#include "Eigen/Sparse"
#include "Eigen/SparseLU"
#include "../Common/ToolKit.h"

namespace MagicDGP
{
    ReliefGeneration::ReliefGeneration(int resX, int resY, Real minX, Real maxX, Real minY, Real maxY) : 
        mResolutionX(resX),
        mResolutionY(resY),
        mMinX(minX),
        mMaxX(maxX),
        mMinY(minY),
        mMaxY(maxY)
    {
    }

    ReliefGeneration::~ReliefGeneration()
    {
    }

    Mesh3D* ReliefGeneration::PlaneReliefFromPointCloud(const Point3DSet* pPC)
    {
        InitHeightField(pPC);
        CompressHeightField();
        //Generate relief mesh
        Real deltaX = (mMaxX - mMinX) / mResolutionX;
        Real deltaY = (mMaxY - mMinY) / mResolutionY;
        Mesh3D* pMesh = new Mesh3D;
        for (int xid = 0; xid < mResolutionX + 1; xid++)
        {
            for (int yid = 0; yid < mResolutionY + 1; yid++)
            {
                Vector3 pos(mMinX + deltaX * xid, mMinY + deltaY * yid, mHeightField.at(xid * (mResolutionY + 1) + yid));
                pMesh->InsertVertex(pos);
            }
        }
        for (int xid = 0; xid < mResolutionX; xid++)
        {
            for (int yid = 0; yid < mResolutionY; yid++)
            {
                int index = xid * (mResolutionY + 1) + yid;
                int indexRight = index + mResolutionY + 1;
                int indexDown = index + 1;
                int indexDiag = indexRight + 1;
                std::vector<Vertex3D* > triList;
                triList.push_back( pMesh->GetVertex(index) );
                triList.push_back( pMesh->GetVertex(indexRight) );
                triList.push_back( pMesh->GetVertex(indexDiag) );
                pMesh->InsertFace(triList);
                triList.clear();
                triList.push_back( pMesh->GetVertex(index) );
                triList.push_back( pMesh->GetVertex(indexDiag) );
                triList.push_back( pMesh->GetVertex(indexDown) );
                pMesh->InsertFace(triList);
            }
        }
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Construct Down Position" << std::endl;
        Real thick = -0.1;
        for (int xid = 0; xid < mResolutionX + 1; xid++)
        {
            for (int yid = 0; yid < mResolutionY + 1; yid++)
            {
                Vector3 pos(mMinX + deltaX * xid, mMinY + deltaY * yid, thick);
                pMesh->InsertVertex(pos);
            }
        }
        int baseIndex = (mResolutionX + 1) * (mResolutionY + 1);
        for (int xid = 0; xid < mResolutionX; xid++)
        {
            for (int yid = 0; yid < mResolutionY; yid++)
            {
                int index = baseIndex + xid * (mResolutionY + 1) + yid;
                int indexRight = index + mResolutionY + 1;
                int indexDown = index + 1;
                int indexDiag = indexRight + 1;
                std::vector<Vertex3D* > triList;
                triList.push_back( pMesh->GetVertex(index) );
                triList.push_back( pMesh->GetVertex(indexDiag) );
                triList.push_back( pMesh->GetVertex(indexRight) );
                pMesh->InsertFace(triList);
                triList.clear();
                triList.push_back( pMesh->GetVertex(index) );
                triList.push_back( pMesh->GetVertex(indexDown) );
                triList.push_back( pMesh->GetVertex(indexDiag) );
                pMesh->InsertFace(triList);
            }
        }
        for (int xid = 0; xid < mResolutionX; xid++)
        {
            int index = xid * (mResolutionY + 1);
            int indexRight = index + mResolutionY + 1;
            int indexDown = index + baseIndex;
            int indexDiag = indexRight + baseIndex;
            std::vector<Vertex3D* > triList;
            triList.push_back( pMesh->GetVertex(index) );
            triList.push_back( pMesh->GetVertex(indexDiag) );
            triList.push_back( pMesh->GetVertex(indexRight) );
            pMesh->InsertFace(triList);
            triList.clear();
            triList.push_back( pMesh->GetVertex(index) );
            triList.push_back( pMesh->GetVertex(indexDown) );
            triList.push_back( pMesh->GetVertex(indexDiag) );
            pMesh->InsertFace(triList);

            index = xid * (mResolutionY + 1) + mResolutionY;
            indexRight = index + mResolutionY + 1;
            indexDown = index + baseIndex;
            indexDiag = indexRight + baseIndex;
            triList.clear();
            triList.push_back( pMesh->GetVertex(index) );
            triList.push_back( pMesh->GetVertex(indexRight) );
            triList.push_back( pMesh->GetVertex(indexDiag) );
            pMesh->InsertFace(triList);
            triList.clear();
            triList.push_back( pMesh->GetVertex(index) );
            triList.push_back( pMesh->GetVertex(indexDiag) );
            triList.push_back( pMesh->GetVertex(indexDown) );
            pMesh->InsertFace(triList);
        }
        for (int yid = 0; yid < mResolutionY; yid++)
        {
            int index = yid;
            int indexRight = index + 1;
            int indexDown = index + baseIndex;
            int indexDiag = indexRight + baseIndex;
            std::vector<Vertex3D* > triList;
            triList.push_back( pMesh->GetVertex(index) );
            triList.push_back( pMesh->GetVertex(indexRight) );
            triList.push_back( pMesh->GetVertex(indexDiag) );
            pMesh->InsertFace(triList);
            triList.clear();
            triList.push_back( pMesh->GetVertex(index) );
            triList.push_back( pMesh->GetVertex(indexDiag) );
            triList.push_back( pMesh->GetVertex(indexDown) );
            pMesh->InsertFace(triList);

            index = mResolutionX * (mResolutionY + 1) + yid;
            indexRight = index + 1;
            indexDown = index + baseIndex;
            indexDiag = indexRight + baseIndex;
            triList.clear();
            triList.push_back( pMesh->GetVertex(index) );
            triList.push_back( pMesh->GetVertex(indexDiag) );
            triList.push_back( pMesh->GetVertex(indexRight) );
            pMesh->InsertFace(triList);
            triList.clear();
            triList.push_back( pMesh->GetVertex(index) );
            triList.push_back( pMesh->GetVertex(indexDown) );
            triList.push_back( pMesh->GetVertex(indexDiag) );
            pMesh->InsertFace(triList);
        }
        pMesh->UpdateNormal();
        return pMesh;
    }

    Mesh3D* ReliefGeneration::CylinderReliefFromPointCloud(const Point3DSet* pPC)
    {
        InitHeightField(pPC);
        CompressHeightField();
        //Generate relief mesh
        Real deltaX = (mMaxX - mMinX) / mResolutionX;
        Real deltaY = (mMaxY - mMinY) / mResolutionY;
        Mesh3D* pMesh = new Mesh3D;
        Real deltaTheta = 3.14159265 / mResolutionX;
        Real baseRadius = (mMaxX - mMinX) / 2;
        for (int xid = 0; xid < mResolutionX + 1; xid++)
        {
            for (int yid = 0; yid < mResolutionY + 1; yid++)
            {
                Real radius = baseRadius + mHeightField.at(xid * (mResolutionY + 1) + yid);
                Real theta = deltaTheta * xid;
                Vector3 pos(radius * cos(theta), mMinY + deltaY * yid, radius * sin(theta));
                //Vector3 pos(mMinX + deltaX * xid, mMinY + deltaY * yid, mHeightField.at(xid * (mResolutionY + 1) + yid));
                pMesh->InsertVertex(pos);
            }
        }
        for (int xid = 0; xid < mResolutionX; xid++)
        {
            for (int yid = 0; yid < mResolutionY; yid++)
            {
                int index = xid * (mResolutionY + 1) + yid;
                int indexRight = index + mResolutionY + 1;
                int indexDown = index + 1;
                int indexDiag = indexRight + 1;
                std::vector<Vertex3D* > triList;
                triList.push_back( pMesh->GetVertex(index) );
                triList.push_back( pMesh->GetVertex(indexDiag) );
                triList.push_back( pMesh->GetVertex(indexRight) );
                pMesh->InsertFace(triList);
                triList.clear();
                triList.push_back( pMesh->GetVertex(index) );
                triList.push_back( pMesh->GetVertex(indexDown) );
                triList.push_back( pMesh->GetVertex(indexDiag) );
                pMesh->InsertFace(triList);
            }
        }
        pMesh->UpdateNormal();
        return pMesh;
    }

    void ReliefGeneration::InitHeightField(const Point3DSet* pPC)
    {
        mHeightField = std::vector<Real>((mResolutionX + 1) * (mResolutionY + 1), 0);
        int pcNum = pPC->GetPointNumber();
        Real minZ = 1.0e10;
        Real deltaX = (mMaxX - mMinX) / mResolutionX;
        Real deltaY = (mMaxY - mMinY) / mResolutionY;
        std::set<int> validIndex;
        std::vector<bool> validFlag((mResolutionX + 1) * (mResolutionY + 1), 0);
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Relief: deltaX: " << deltaX << " deltaY: " << deltaY << std::endl;
        //Init Height Field
        for (int i = 0; i < pcNum; i++)
        {
            Vector3 pos = pPC->GetPoint(i)->GetPosition();
            if (pos[2] < minZ)
            {
                minZ = pos[2];
            }
            int xIndex = (pos[0] - mMinX) / deltaX;
            int yIndex = (pos[1] - mMinY) / deltaY;
            if (xIndex < 0 || xIndex > mResolutionX || yIndex < 0 || yIndex > mResolutionY)
            {
                continue;
            }
            int heightIndex = xIndex * (mResolutionY + 1) + yIndex;
            mHeightField.at(heightIndex) = pos[2];
            validIndex.insert(heightIndex);
            validFlag[heightIndex] = 1;
        }
        for (std::set<int>::iterator itr = validIndex.begin(); itr != validIndex.end(); ++itr)
        {
            mHeightField.at(*itr) -= minZ;
        }
        //Fill Hole
        for (int itrIdx = 0; itrIdx < 2; itrIdx++)
        {
            std::vector<int> newValidIndex;
            for (int xid = 1; xid < mResolutionX; xid++)
            {
                for (int yid = 1; yid < mResolutionY; yid++)
                {
                    int index = xid * (mResolutionY + 1) + yid;
                    if (validFlag[index] != 1)
                    {
                        Real height = 0;
                        int validNeighborNum = 0;
                        if (validFlag[index - mResolutionY - 1] == 1)
                        {
                            height += mHeightField.at(index - mResolutionY - 1);
                            validNeighborNum++;
                        }
                        if (validFlag[index + mResolutionY + 1] == 1)
                        {
                            height += mHeightField.at(index + mResolutionY + 1);
                            validNeighborNum++;
                        }
                        if (validFlag[index - 1] == 1)
                        {
                            height += mHeightField.at(index - 1);
                            validNeighborNum++;
                        }
                        if (validFlag[index + 1] == 1)
                        {
                            height += mHeightField.at(index + 1);
                            validNeighborNum++;
                        }
                        //result
                        if (validNeighborNum > 0)
                        {
                            height /= validNeighborNum;
                            mHeightField.at(index) = height;
                            newValidIndex.push_back(index);
                        }
                    }
                }
            }
            for (std::vector<int>::iterator itr = newValidIndex.begin(); itr != newValidIndex.end(); ++itr)
            {
                validFlag[*itr] = 1;
            }
        }
    }

    void ReliefGeneration::CompressHeightField()
    {
        Real bbScale = mMaxX - mMinX;
        Real alpha = bbScale * 250.0;
        Real threthold = bbScale * 0.05;
        int vertNum = (mResolutionX + 1) * (mResolutionY + 1);
        std::vector<Vector3> DeltaVector(vertNum);
        for (int xid = 0; xid < mResolutionX; xid++)
        {
            for (int yid = 0; yid < mResolutionY; yid++)
            {
                int index = xid * (mResolutionY + 1) + yid;
                Vector3 deltaT(0, 0, 0);
                deltaT[0] = mHeightField.at(index + mResolutionY + 1) - mHeightField.at(index);
                deltaT[1] = mHeightField.at(index + 1) - mHeightField.at(index);
                Real deltaMag = deltaT.Normalise();
                if (deltaMag > threthold)
                {
                    deltaMag = 0;
                }
                else
                {
                    deltaMag = log(1 + alpha * deltaMag) / alpha;
                }
                DeltaVector.at(index) = deltaT * deltaMag;
            }
        }
        std::vector<Real> LaplaceField(vertNum);
        for (int xid = 1; xid < mResolutionX; xid++)
        {
            for (int yid = 1; yid < mResolutionY; yid++)
            {
                int index = xid * (mResolutionY + 1) + yid;
                LaplaceField.at(index) = DeltaVector.at(index)[0] - DeltaVector.at(index - mResolutionY - 1)[0] + 
                    DeltaVector.at(index)[1] - DeltaVector.at(index - 1)[1];

            }
        }
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Relief: Construct Matrix" << std::endl;
        std::vector< Eigen::Triplet<double> > tripletList;
        Eigen::VectorXd b(vertNum, 1);
        for (int xid = 0; xid < mResolutionX + 1; xid++)
        {
            for (int yid = 0; yid < mResolutionY + 1; yid++)
            {
                int index = xid * (mResolutionY + 1) + yid;
                if (xid == 0 || xid == mResolutionX || yid == 0 || yid == mResolutionY)
                {
                    tripletList.push_back( Eigen::Triplet<double>(index, index, 1) );
                    b(index) = 0;
                }
                else
                {
                    tripletList.push_back( Eigen::Triplet<double>(index, index, -4.0) );
                    tripletList.push_back( Eigen::Triplet<double>(index, index + 1, 1.0) );
                    tripletList.push_back( Eigen::Triplet<double>(index, index - 1, 1.0) );
                    tripletList.push_back( Eigen::Triplet<double>(index, index + mResolutionY + 1, 1.0) );
                    tripletList.push_back( Eigen::Triplet<double>(index, index - mResolutionY - 1, 1.0) );
                    b(index) = LaplaceField.at(index);
                }
            }
        }
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Relief: Solve Matrix" << std::endl;
        Eigen::SparseMatrix<double, Eigen::ColMajor> matA(vertNum,vertNum);
        matA.setFromTriplets(tripletList.begin(), tripletList.end());
        Eigen::SparseLU<Eigen::SparseMatrix<double, Eigen::ColMajor> > solver;
        solver.compute(matA);
        if(solver.info()!= Eigen::Success) 
        {
            MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Relief: SuperLU Failed" << std::endl;
        }
        Eigen::VectorXd res = solver.solve(b);
        //Copy results
        for (int i = 0; i < vertNum; i++)
        {
            mHeightField.at(i) = res(i);
        }
    }
}
#include "Relief.h"
#include <set>
#include "Eigen/Sparse"
#include "Eigen/SparseLU"
#include "../Common/ToolKit.h"
#include "Tool/LogSystem.h"

namespace MagicDGP
{
    ReliefGeneration::ReliefGeneration() 
    {
    }

    ReliefGeneration::~ReliefGeneration()
    {
    }

    LightMesh3D* ReliefGeneration::PlaneReliefFromHeightField(std::vector<double>& heightField, int resX, int resY)
    {
        CompressHeightField(heightField, resX, resY);
        //Generate relief mesh
        double minX = -1.0;
        double maxX = 1.0;
        double minY = -1.0;
        double maxY = 1.0;
        double deltaX = (maxX - minX) / resX;
        double deltaY = (maxY - minY) / resY;
        LightMesh3D* pMesh = new LightMesh3D;
        for (int xid = 0; xid < resX + 1; xid++)
        {
            for (int yid = 0; yid < resY + 1; yid++)
            {
                MagicMath::Vector3 pos(minX + deltaX * xid, minY + deltaY * yid, heightField.at(xid * (resY + 1) + yid));
                pMesh->InsertVertex(pos);
            }
        }
        for (int xid = 0; xid < resX; xid++)
        {
            for (int yid = 0; yid < resY; yid++)
            {
                int index = xid * (resY + 1) + yid;
                int indexRight = index + resY + 1;
                int indexDown = index + 1;
                int indexDiag = indexRight + 1;
                FaceIndex faceIdx;
                faceIdx.mIndex[0] = index;
                faceIdx.mIndex[1] = indexRight;
                faceIdx.mIndex[2] = indexDiag;
                pMesh->InsertFace(faceIdx);
                faceIdx.mIndex[0] = index;
                faceIdx.mIndex[1] = indexDiag;
                faceIdx.mIndex[2] = indexDown;
                pMesh->InsertFace(faceIdx);
            }
        }

        double thick = -0.1;
        for (int xid = 0; xid < resX + 1; xid++)
        {
            for (int yid = 0; yid < resY + 1; yid++)
            {
                MagicMath::Vector3 pos(minX + deltaX * xid, minY + deltaY * yid, thick);
                pMesh->InsertVertex(pos);
            }
        }
        int baseIndex = (resX + 1) * (resY + 1);
        for (int xid = 0; xid < resX; xid++)
        {
            for (int yid = 0; yid < resY; yid++)
            {
                int index = baseIndex + xid * (resY + 1) + yid;
                int indexRight = index + resY + 1;
                int indexDown = index + 1;
                int indexDiag = indexRight + 1;

                FaceIndex faceIdx;
                faceIdx.mIndex[0] = index;
                faceIdx.mIndex[1] = indexDiag;
                faceIdx.mIndex[2] = indexRight;
                pMesh->InsertFace(faceIdx);

                faceIdx.mIndex[0] = index;
                faceIdx.mIndex[1] = indexDown;
                faceIdx.mIndex[2] = indexDiag;
                pMesh->InsertFace(faceIdx);
            }
        }
        for (int xid = 0; xid < resX; xid++)
        {
            int index = xid * (resY + 1);
            int indexRight = index + resY + 1;
            int indexDown = index + baseIndex;
            int indexDiag = indexRight + baseIndex;

            FaceIndex faceIdx;
            faceIdx.mIndex[0] = index;
            faceIdx.mIndex[1] = indexDiag;
            faceIdx.mIndex[2] = indexRight;
            pMesh->InsertFace(faceIdx);

            faceIdx.mIndex[0] = index;
            faceIdx.mIndex[1] = indexDown;
            faceIdx.mIndex[2] = indexDiag;
            pMesh->InsertFace(faceIdx);

            index = xid * (resY + 1) + resY;
            indexRight = index + resY + 1;
            indexDown = index + baseIndex;
            indexDiag = indexRight + baseIndex;

            faceIdx.mIndex[0] = index;
            faceIdx.mIndex[1] = indexRight;
            faceIdx.mIndex[2] = indexDiag;
            pMesh->InsertFace(faceIdx);

            faceIdx.mIndex[0] = index;
            faceIdx.mIndex[1] = indexDiag;
            faceIdx.mIndex[2] = indexDown;
            pMesh->InsertFace(faceIdx);
        }
        for (int yid = 0; yid < resY; yid++)
        {
            int index = yid;
            int indexRight = index + 1;
            int indexDown = index + baseIndex;
            int indexDiag = indexRight + baseIndex;

            FaceIndex faceIdx;
            faceIdx.mIndex[0] = index;
            faceIdx.mIndex[1] = indexRight;
            faceIdx.mIndex[2] = indexDiag;
            pMesh->InsertFace(faceIdx);

            faceIdx.mIndex[0] = index;
            faceIdx.mIndex[1] = indexDiag;
            faceIdx.mIndex[2] = indexDown;
            pMesh->InsertFace(faceIdx);

            index = resX * (resY + 1) + yid;
            indexRight = index + 1;
            indexDown = index + baseIndex;
            indexDiag = indexRight + baseIndex;

            faceIdx.mIndex[0] = index;
            faceIdx.mIndex[1] = indexDiag;
            faceIdx.mIndex[2] = indexRight;
            pMesh->InsertFace(faceIdx);

            faceIdx.mIndex[0] = index;
            faceIdx.mIndex[1] = indexDown;
            faceIdx.mIndex[2] = indexDiag;
            pMesh->InsertFace(faceIdx);
        }
        pMesh->UpdateNormal();

        return pMesh;
    }

    LightMesh3D* ReliefGeneration::CylinderReliefFromHeightField(std::vector<double>& heightField, int resX, int resY)
    {
        CompressHeightField(heightField, resX, resY);
        //Generate relief mesh
        double minX = -1.0;
        double maxX = 1.0;
        double minY = -1.0;
        double maxY = 1.0;
        double deltaX = (maxX - minX) / resX;
        double deltaY = (maxY - minY) / resY;
        LightMesh3D* pMesh = new LightMesh3D;
        double deltaTheta = 3.14159265 / resX;
        double baseRadius = (maxX - minX) / 2;
        for (int xid = 0; xid < resX + 1; xid++)
        {
            for (int yid = 0; yid < resY + 1; yid++)
            {
                double radius = baseRadius + heightField.at(xid * (resY + 1) + yid);
                double theta = deltaTheta * xid;
                MagicMath::Vector3 pos(radius * cos(theta), minY + deltaY * yid, radius * sin(theta));
                pMesh->InsertVertex(pos);
            }
        }
        for (int xid = 0; xid < resX; xid++)
        {
            for (int yid = 0; yid < resY; yid++)
            {
                int index = xid * (resY + 1) + yid;
                int indexRight = index + resY + 1;
                int indexDown = index + 1;
                int indexDiag = indexRight + 1;
                /*std::vector<Vertex3D* > triList;
                triList.push_back( pMesh->GetVertex(index) );
                triList.push_back( pMesh->GetVertex(indexDiag) );
                triList.push_back( pMesh->GetVertex(indexRight) );*/
                FaceIndex faceIdx;
                faceIdx.mIndex[0] = index;
                faceIdx.mIndex[1] = indexDiag;
                faceIdx.mIndex[2] = indexRight;
                pMesh->InsertFace(faceIdx);
                /*triList.clear();
                triList.push_back( pMesh->GetVertex(index) );
                triList.push_back( pMesh->GetVertex(indexDown) );
                triList.push_back( pMesh->GetVertex(indexDiag) );*/
                faceIdx.mIndex[0] = index;
                faceIdx.mIndex[1] = indexDown;
                faceIdx.mIndex[2] = indexDiag;
                pMesh->InsertFace(faceIdx);
            }
        }
        pMesh->UpdateNormal();
        return pMesh;
    }

    void ReliefGeneration::CompressHeightField(std::vector<double>& heightField, int resX, int resY)
    {
        double bbScale = 2;
        double alpha = bbScale * 300.0;
        double threthold = bbScale * 0.05;
        int vertNum = (resX + 1) * (resY + 1);
        std::vector<MagicMath::Vector3> DeltaVector(vertNum);
        for (int xid = 0; xid < resX; xid++)
        {
            for (int yid = 0; yid < resY; yid++)
            {
                int index = xid * (resY + 1) + yid;
                MagicMath::Vector3 deltaT(0, 0, 0);
                deltaT[0] = heightField.at(index + resY + 1) - heightField.at(index);
                deltaT[1] = heightField.at(index + 1) - heightField.at(index);
                double deltaMag = deltaT.Normalise();
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
        std::vector<double> LaplaceField(vertNum);
        for (int xid = 1; xid < resX; xid++)
        {
            for (int yid = 1; yid < resY; yid++)
            {
                int index = xid * (resY + 1) + yid;
                LaplaceField.at(index) = DeltaVector.at(index)[0] - DeltaVector.at(index - resY - 1)[0] + 
                    DeltaVector.at(index)[1] - DeltaVector.at(index - 1)[1];

            }
        }
        DebugLog << "Relief: Construct Matrix" << std::endl;
        std::vector< Eigen::Triplet<double> > tripletList;
        Eigen::VectorXd b(vertNum, 1);
        for (int xid = 0; xid < resX + 1; xid++)
        {
            for (int yid = 0; yid < resY + 1; yid++)
            {
                int index = xid * (resY + 1) + yid;
                if (xid == 0 || xid == resX || yid == 0 || yid == resY)
                {
                    tripletList.push_back( Eigen::Triplet<double>(index, index, 1) );
                    b(index) = 0;
                }
                else
                {
                    tripletList.push_back( Eigen::Triplet<double>(index, index, -4.0) );
                    tripletList.push_back( Eigen::Triplet<double>(index, index + 1, 1.0) );
                    tripletList.push_back( Eigen::Triplet<double>(index, index - 1, 1.0) );
                    tripletList.push_back( Eigen::Triplet<double>(index, index + resY + 1, 1.0) );
                    tripletList.push_back( Eigen::Triplet<double>(index, index - resY - 1, 1.0) );
                    b(index) = LaplaceField.at(index);
                }
            }
        }
        DebugLog << "Relief: Solve Matrix" << std::endl;
        Eigen::SparseMatrix<double, Eigen::ColMajor> matA(vertNum,vertNum);
        matA.setFromTriplets(tripletList.begin(), tripletList.end());
        Eigen::SparseLU<Eigen::SparseMatrix<double, Eigen::ColMajor> > solver;
        solver.compute(matA);
        if(solver.info()!= Eigen::Success) 
        {
            DebugLog << "Relief: SuperLU Failed" << std::endl;
        }
        Eigen::VectorXd res = solver.solve(b);
        //Copy results
        for (int i = 0; i < vertNum; i++)
        {
            heightField.at(i) = res(i);
        }
    }

}
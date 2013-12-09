#include "Consolidation.h"
#include "flann/flann.h"
#include "Eigen/Eigenvalues"
#include "Eigen/Sparse"
#include "Eigen/SparseLU"

namespace MagicDGP
{
    Consolidation::Consolidation()
    {
    }

    Consolidation::~Consolidation()
    {
    }

    void Consolidation::CalPointSetNormal(Point3DSet* pPointSet)
    {
        int pointNum = pPointSet->GetPointNumber();
        std::vector<Vector3> norList(pointNum);

        int dim = 3;
        int refNum = pointNum;
        float* dataSet = new float[refNum * dim];
        int searchNum = pointNum;
        float* searchSet = new float[searchNum * dim];
        for (int pid = 0; pid < pointNum; pid++)
        {
            MagicDGP::Vector3 pos = pPointSet->GetPoint(pid)->GetPosition();
            dataSet[dim * pid + 0] = pos[0];
            dataSet[dim * pid + 1] = pos[1];
            dataSet[dim * pid + 2] = pos[2];
            searchSet[dim * pid + 0] = pos[0];
            searchSet[dim * pid + 1] = pos[1];
            searchSet[dim * pid + 2] = pos[2];
        }
        int nn = 20;
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

        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> es;
        for (int pid = 0; pid < pointNum; pid++)
        {
            Vector3 pos = pPointSet->GetPoint(pid)->GetPosition();
            Vector3 deltaPos[20]; //nn
            int baseIndex = pid * nn;
            for (int j = 0; j < nn; j++)
            {
                deltaPos[j] = pPointSet->GetPoint(pIndex[baseIndex + j])->GetPosition() - pos;
            }
            Eigen::Matrix3d mat;
            for (int xx = 0; xx < 3; xx++)
            {
                for (int yy = 0; yy < 3; yy++)
                {
                    Real v = 0;
                    for (int kk = 0; kk < nn; kk++)
                    {
                        v += deltaPos[kk][xx] * deltaPos[kk][yy];
                    }
                    mat(xx, yy) = v;
                }
            }//end for xx
            es.compute(mat);
            Eigen::Vector3d norvec = es.eigenvectors().col(0);
            Vector3 nor(norvec(0), norvec(1), norvec(2));
            Real norLen = nor.Normalise();
            if (norLen < Epsilon)
            {
                DebugLog << "Error: small normal length" << std::endl;
            }
            norList.at(pid) = nor;
        }
        if (pDist != NULL)
        {
            delete []pDist;
            pDist = NULL;
        }
        //Make normal consitent
        std::multimap<Real, int> prioritySet;
        std::vector<bool> acceptMark(pointNum, 0);
        while (true)
        {
            //find activeId
            int activeId = -1;
            if (prioritySet.size() > 0)
            {
                std::multimap<Real, int>::iterator firstItr = prioritySet.begin();
                activeId = firstItr->second;
                prioritySet.erase(firstItr);
            }
            else 
            {
                Real maxZ = -1.0e10;
                int maxZId = -1;
                for (int pid = 0; pid < pointNum; pid++)
                {
                    if (acceptMark.at(pid) == 0)
                    {
                        Real z = pPointSet->GetPoint(pid)->GetPosition()[2];
                        if (z > maxZ)
                        {
                            maxZ = z;
                            maxZId = pid;
                        }
                    }
                }
                if (maxZId > -1)
                {
                    if (norList.at(maxZId)[2] < 0)
                    {
                        norList.at(maxZId) *= -1;
                    }
                }
                activeId = maxZId;
                //DebugLog << "new start point" << std::endl;
            }
            if (activeId == -1)
            {
                break;
            }

            acceptMark.at(activeId) = 1;
            //add new point to priority set
            int baseIndex = activeId * nn; 
            for (int nid = 0; nid < nn; nid++)
            {
                int nIndex = pIndex[baseIndex + nid];
                if (acceptMark.at(nIndex) == true)
                {
                    continue;
                }
                else
                {
                    Real cosAngle = norList.at(activeId) * norList.at(nIndex);
                    if (cosAngle < 0)
                    {
                        norList.at(nIndex) *= -1;
                    }
                    prioritySet.insert( std::pair<Real, int>(1.0 - fabs(cosAngle), nIndex) );
                    acceptMark.at(nIndex) = true;
                }
            }
        }

        for (int pid = 0; pid < pointNum; pid++)
        {
            pPointSet->GetPoint(pid)->SetNormal(norList.at(pid));
        }
        //
        if (pIndex != NULL)
        {
            delete []pIndex;
            pIndex = NULL;
        }
        pPointSet->SetHasNormal(true);
    }

    bool Consolidation::RedressPointSetNormal(Point3DSet* pPointSet)
    {
        if (pPointSet->HasNormal() == false)
        {
            WarnLog << "Consolidation::RedressPointSetNormal need point set normal, but this one has no normal" << std::endl;
            return false;
        }

        int pointNum = pPointSet->GetPointNumber();
        int dim = 3;
        int refNum = pointNum;
        float* dataSet = new float[refNum * dim];
        int searchNum = pointNum;
        float* searchSet = new float[searchNum * dim];
        for (int pid = 0; pid < pointNum; pid++)
        {
            MagicDGP::Vector3 pos = pPointSet->GetPoint(pid)->GetPosition();
            dataSet[dim * pid + 0] = pos[0];
            dataSet[dim * pid + 1] = pos[1];
            dataSet[dim * pid + 2] = pos[2];
            searchSet[dim * pid + 0] = pos[0];
            searchSet[dim * pid + 1] = pos[1];
            searchSet[dim * pid + 2] = pos[2];
        }
        int nn = 20;
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

        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> es;
        for (int pid = 0; pid < pointNum; pid++)
        {
            Point3D* pPoint = pPointSet->GetPoint(pid);
            Vector3 pos = pPoint->GetPosition();
            Vector3 deltaPos[20]; //nn
            int baseIndex = pid * nn;
            for (int j = 0; j < nn; j++)
            {
                deltaPos[j] = pPointSet->GetPoint(pIndex[baseIndex + j])->GetPosition() - pos;
            }
            Eigen::Matrix3d mat;
            for (int xx = 0; xx < 3; xx++)
            {
                for (int yy = 0; yy < 3; yy++)
                {
                    Real v = 0;
                    for (int kk = 0; kk < nn; kk++)
                    {
                        v += deltaPos[kk][xx] * deltaPos[kk][yy];
                    }
                    mat(xx, yy) = v;
                }
            }//end for xx
            es.compute(mat);
            Eigen::Vector3d norvec = es.eigenvectors().col(0);
            Vector3 nor(norvec(0), norvec(1), norvec(2));
            Real norLen = nor.Normalise();
            if (norLen < Epsilon)
            {
                DebugLog << "Error: small normal length" << std::endl;
            }
            if (nor * (pPoint->GetNormal()) < 0)
            {
                nor *= (-1);
            }
            pPoint->SetNormal(nor);
        }
        if (pDist != NULL)
        {
            delete []pDist;
            pDist = NULL;
        }
        if (pIndex != NULL)
        {
            delete []pIndex;
            pIndex = NULL;
        }
    }

    Mesh3D* Consolidation::RemoveSmallMeshPatch(Mesh3D* pMesh, Real proportion)
    {
        DebugLog << "Consolidation::RemoveSmallMeshPatch" << std::endl;
        int vertNum = pMesh->GetVertexNumber();
        int smallNum = vertNum * proportion;
        if (smallNum < 10)
        {
            return NULL;
        }
        std::vector<bool> visitFlag(vertNum, 0);
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
        std::map<int, int> vertMapOld2New;
        Mesh3D* pNewMesh = new Mesh3D;
        int newMeshVertIndex = 0;
        std::vector<bool> vertValidFlag(vertNum, 0);
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

    Point3DSet* Consolidation::RemovePointSetOutlier(Point3DSet* pPS, Real proportion)
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

    void Consolidation::SimpleMeshSmooth(Mesh3D* pMesh)
    {
        DebugLog << "Consolidation::SimpleMeshSmooth...." << std::endl;
        pMesh->UpdateBoundaryFlag();
        int vertNum = pMesh->GetVertexNumber();
        std::vector<Vector3> posBakList(vertNum);
        for (int vid = 0; vid < vertNum; vid++)
        {
            posBakList.at(vid) = pMesh->GetVertex(vid)->GetPosition();
        }
        Real smoothWeight = 0.75;
        std::vector<Vector3> smoothPos(vertNum);
        for (int vid = 0; vid < vertNum; vid++)
        {
            Vertex3D* pVert = pMesh->GetVertex(vid);
            if (pVert->GetBoundaryType() == BT_Boundary)
            {
                continue;
            }
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
            smoothPos.at(vid) = avgPos * smoothWeight + pVert->GetPosition() * (1 - smoothWeight);
            //avgPos = avgPos * smoothWeight + pVert->GetPosition() * (1 - smoothWeight);
            //pVert->SetPosition(avgPos);
        }
        for (int vid = 0; vid < vertNum; vid++)
        {
            Vertex3D* pVert = pMesh->GetVertex(vid);
            if (pVert->GetBoundaryType() == BT_Boundary)
            {
                continue;
            }
            pVert->SetPosition(smoothPos.at(vid));
        }
        pMesh->UpdateNormal();
    }

    void Consolidation::MeanCurvatureFlowFairing(Mesh3D* pMesh)
    {
        pMesh->UpdateBoundaryFlag();
        pMesh->CalculateFaceArea();
        int vertNum = pMesh->GetVertexNumber();
        std::vector< Eigen::Triplet<double> > tripletList;
        Eigen::VectorXd bx(vertNum, 1);
        Eigen::VectorXd by(vertNum, 1);
        Eigen::VectorXd bz(vertNum, 1);
        Real deltaT = 0.00005;
        Real epsilon = 1.0e-5;
        for (int vid = 0; vid < vertNum; vid++)
        {
            Vertex3D* pVert = pMesh->GetVertex(vid);
            if (pVert->GetBoundaryType() == BT_Boundary)
            {
                tripletList.push_back( Eigen::Triplet<double>(vid, vid, 1) );
                Vector3 pos = pVert->GetPosition();
                bx(vid) = pos[0];
                by(vid) = pos[1];
                bz(vid) = pos[2];
            }
            else
            {
                Real wSum = 0.0;
                Real areaSum = 0.0;
                Edge3D* pEdge = pVert->GetEdge();
                do
                {
                    areaSum += pEdge->GetFace()->GetArea();
                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != NULL && pEdge != pVert->GetEdge());
                if (areaSum < epsilon)
                {
                    tripletList.push_back( Eigen::Triplet<double>(vid, vid, 1) );
                    Vector3 pos = pVert->GetPosition();
                    bx(vid) = pos[0];
                    by(vid) = pos[1];
                    bz(vid) = pos[2];
                    continue;
                }
                areaSum = areaSum < epsilon ? epsilon : areaSum;
                pEdge = pVert->GetEdge();
                do
                {
                    Real wTemp = 0.0;
                    Real sinV, cosV;
                    if (pEdge->GetFace() == NULL)
                    {
                        DebugLog << "pEdge->GetFace() == NULL" << std::endl;
                    }
                    if (pEdge->GetPair()->GetFace() == NULL)
                    {
                        DebugLog << "pEdge->GetPair()->GetFace() == NULL" << std::endl;
                    }
                    Vector3 dir0 = pEdge->GetVertex()->GetPosition() - pEdge->GetNext()->GetVertex()->GetPosition();
                    dir0.Normalise();
                    Vector3 dir1 = pEdge->GetPre()->GetVertex()->GetPosition() - pEdge->GetNext()->GetVertex()->GetPosition();
                    dir1.Normalise();
                    cosV = dir0 * dir1;
                    cosV = (cosV > 1) ? 1 : ((cosV < -1) ? -1 : cosV); 
                    sinV = sqrt(1 - cosV * cosV);
                    sinV = (sinV < epsilon) ? epsilon : sinV;
                    wTemp += cosV / sinV;

                    dir0 = pEdge->GetPair()->GetVertex()->GetPosition() - pEdge->GetPair()->GetNext()->GetVertex()->GetPosition();
                    dir0.Normalise();
                    dir1 = pEdge->GetPair()->GetPre()->GetVertex()->GetPosition() - pEdge->GetPair()->GetNext()->GetVertex()->GetPosition();
                    dir1.Normalise();
                    cosV = dir0 * dir1;
                    cosV = (cosV > 1) ? 1 : ((cosV < -1) ? -1 : cosV);
                    sinV = sqrt(1 - cosV * cosV);
                    sinV = (sinV < epsilon) ? epsilon : sinV;
                    wTemp += cosV / sinV; 

                    wSum += wTemp;
                    tripletList.push_back( Eigen::Triplet<double>(vid, pEdge->GetVertex()->GetId() , wTemp * (-1) * deltaT / areaSum) );

                    pEdge = pEdge->GetPair()->GetNext();
                } while (pEdge != NULL && pEdge != pVert->GetEdge());
                tripletList.push_back( Eigen::Triplet<double>(vid, vid, 1.0 + deltaT * wSum / areaSum) );
                Vector3 pos = pVert->GetPosition();
                bx(vid) = pos[0];
                by(vid) = pos[1];
                bz(vid) = pos[2];
            }
        }
        DebugLog << "Create MatA Coeffecient" << std::endl;
        Eigen::SparseMatrix<double, Eigen::ColMajor> matA(vertNum,vertNum);
        matA.setFromTriplets(tripletList.begin(), tripletList.end());
        Eigen::SparseLU<Eigen::SparseMatrix<double, Eigen::ColMajor> > solver;
        solver.compute(matA);
        if(solver.info()!= Eigen::Success) 
        {
            MagicLog(MagicCore::LOGLEVEL_DEBUG) << "Relief: SuperLU Failed" << std::endl;
        }
        Eigen::VectorXd resX = solver.solve(bx);
        Eigen::VectorXd resY = solver.solve(by);
        Eigen::VectorXd resZ = solver.solve(bz);
        //Copy result
        for (int vid = 0; vid < vertNum; vid++)
        {
            Vector3 newPos(resX(vid), resY(vid), resZ(vid));
            pMesh->GetVertex(vid)->SetPosition(newPos);
        }
        DebugLog << "Fisish mean curvature flow" << std::endl;
        pMesh->UpdateNormal();
    }

    void Consolidation::SimplePointsetSmooth(Point3DSet* pPS)
    {
        int pointNum = pPS->GetPointNumber();

        int dim = 3;
        int refNum = pointNum;
        float* dataSet = new float[refNum * dim];
        int searchNum = pointNum;
        float* searchSet = new float[searchNum * dim];
        for (int pid = 0; pid < pointNum; pid++)
        {
            MagicDGP::Vector3 pos = pPS->GetPoint(pid)->GetPosition();
            dataSet[dim * pid + 0] = pos[0];
            dataSet[dim * pid + 1] = pos[1];
            dataSet[dim * pid + 2] = pos[2];
            searchSet[dim * pid + 0] = pos[0];
            searchSet[dim * pid + 1] = pos[1];
            searchSet[dim * pid + 2] = pos[2];
        }
        int nn = 20;
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

        Real smoothWeight = 0.75;
        std::vector<Vector3> smoothPos(pointNum);
        for (int pid = 0; pid < pointNum; pid++)
        {
            Vector3 pos = pPS->GetPoint(pid)->GetPosition();
            Vector3 avgPos(0, 0, 0);
            int baseIndex = pid * nn;
            for (int j = 0; j < nn; j++)
            {
                avgPos += pPS->GetPoint(pIndex[baseIndex + j])->GetPosition();
            }
            avgPos /= nn;
            smoothPos.at(pid) = avgPos * smoothWeight + pos * (1 - smoothWeight);
        }
        for (int pid = 0; pid < pointNum; pid++)
        {
            pPS->GetPoint(pid)->SetPosition(smoothPos.at(pid));
        }
        if (pDist != NULL)
        {
            delete []pDist;
            pDist = NULL;
        }
        if (pIndex != NULL)
        {
            delete []pIndex;
            pIndex = NULL;
        }
    }
}
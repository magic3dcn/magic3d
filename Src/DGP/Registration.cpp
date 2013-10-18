//#include "StdAfx.h"
#include "Registration.h"
#include "Eigen/Dense"
#include "Eigen/Geometry"
//#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"

namespace MagicDGP
{
    Registration::Registration() : 
        mFlannIndex(NULL),
        mDataSet(NULL)
    {
    }

    Registration::~Registration()
    {
        if (mFlannIndex != NULL)
        {
            flann_free_index(mFlannIndex, &mSearchPara);
        }
        if (mDataSet != NULL)
        {
            delete []mDataSet;
        }
    }

    void Registration::ICPRegistrate(const Point3DSet* pRef, Point3DSet* pOrigin, const HomoMatrix4* pTransInit, HomoMatrix4* pTransRes)
    {
        int iterNum = 10;
        *pTransRes = *pTransInit;
        //MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("newPC", "SimplePoint_Green", pOrigin, *pTransRes);
        //MagicCore::RenderSystem::GetSingleton()->Update();
        ICPInitRefData(pRef);
        std::vector<int> sampleIndex;
        ICPSamplePoint(pOrigin, sampleIndex);
        for (int k = 0; k < iterNum; k++)
        {
            float timeCorres = MagicCore::ToolKit::GetSingleton()->GetTime();
            std::vector<int> correspondIndex;
            ICPFindCorrespondance(pRef, pOrigin, pTransRes, sampleIndex, correspondIndex);
            MagicLog << "        ICPCorres: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeCorres << std::endl;
            float timeMinimize = MagicCore::ToolKit::GetSingleton()->GetTime();
            HomoMatrix4 transDelta;
            ICPEnergyMinimization(pRef, pOrigin, pTransRes, sampleIndex, correspondIndex, &transDelta);
            MagicLog << "        ICPMinimize: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeMinimize << std::endl;
            //*pTransRes *= transDelta;
            *pTransRes = transDelta * (*pTransRes);
            //MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("newPC", "SimplePoint_Green", pOrigin, *pTransRes);
            //MagicCore::RenderSystem::GetSingleton()->Update();
            //determine if stop
            //if (k > 4 && 
            //    fabs(transDelta.GetValue(0, 3)) < 1.f && 
            //    fabs(transDelta.GetValue(1, 3)) < 1.f && 
            //    fabs(transDelta.GetValue(2, 3)) < 1.f)
            //{
            //    MagicLog << "ICP iterator number: " << k + 1 << std::endl;
            //    break;
            //}
            if (fabs(transDelta.GetValue(0, 3)) < 0.01f && 
                fabs(transDelta.GetValue(1, 3)) < 0.01f && 
                fabs(transDelta.GetValue(2, 3)) < 0.01f)
            {
                MagicLog << "ICP iterator number: " << k + 1 << std::endl;
                break;
            }
        }
    }

    void Registration::ICPSamplePoint(const Point3DSet* pPC, std::vector<int>& sampleIndex)
    {
        //MagicLog << "Registration::ICPSamplePoint" << std::endl;
        int pcNum = pPC->GetPointNumber();
        static int startIndex = 0;
        int sampleNum;
        if (pcNum > 5000)
        {
            sampleNum = 5000;
        }
        else
        {
            sampleNum = pcNum;
        }

        int deltaSize = pcNum / sampleNum;
        if (deltaSize < 1)
        {
            deltaSize = 1;
        }
        for (int i = startIndex; i < pcNum; i += deltaSize)
        {
            sampleIndex.push_back(i);
        }
        startIndex++;
        startIndex = startIndex % deltaSize;

        //int pcNum = pPC->GetPointNumber();
        //std::map<Real, int> normalDistribute;
        //for (int i = 0; i < pcNum; i++)
        //{
        //    Vector3 nor = pPC->GetPoint(i)->GetNormal();
        //    normalDistribute[nor[2]] = i;
        //}
        //int sampleNum = 1000;
        //int currentNum = 0;
        //for (std::map<Real, int>::iterator itr = normalDistribute.begin(); itr != normalDistribute.end(); ++itr)
        //{
        //    sampleIndex.push_back(itr->second);
        //    currentNum++;
        //    if (currentNum == sampleNum)
        //    {
        //        break;
        //    }
        //}

        //std::vector<Vector3> startPos, endPos;
        //for (int i = 0; i < sampleIndex.size(); i++)
        //{
        //    Vector3 pos = pPC->GetPoint(sampleIndex.at(i))->GetPosition();
        //    Vector3 nor = pPC->GetPoint(sampleIndex.at(i))->GetNormal();
        //    startPos.push_back(pos);
        //    endPos.push_back(pos + nor * 50);
        //}
        //MagicCore::RenderSystem::GetSingleton()->RenderLineSegments("testLine", "SimpleLine", startPos, endPos);
        //MagicCore::RenderSystem::GetSingleton()->Update();
    }

    void Registration::ICPInitRefData(const Point3DSet* pRef)
    {
        float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        int dim = 3;
        int refNum = pRef->GetPointNumber();
        mDataSet = new float[refNum * dim];
        for (int i = 0; i < refNum; i++)
        {
            Vector3 pos = pRef->GetPoint(i)->GetPosition();
            mDataSet[dim * i + 0] = pos[0];
            mDataSet[dim * i + 1] = pos[1];
            mDataSet[dim * i + 2] = pos[2];
        }
        mSearchPara = DEFAULT_FLANN_PARAMETERS;
        mSearchPara.algorithm = FLANN_INDEX_KDTREE;
        mSearchPara.trees = 8;
        mSearchPara.log_level = FLANN_LOG_INFO;
	    mSearchPara.checks = 64;
        float speedup;
        mFlannIndex = flann_build_index(mDataSet, refNum, dim, &speedup, &mSearchPara);
        MagicLog << "      ICPInitRefData: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;
    }

    void Registration::ICPFindCorrespondance(const Point3DSet* pRef, const Point3DSet* pOrigin, const HomoMatrix4* pTransInit,
            std::vector<int>& sampleIndex,  std::vector<int>& correspondIndex)
    {
        //MagicLog << "Registration::ICPFindCorrespondance" << std::endl;
        //float timeStart = MagicCore::ToolKit::GetSingleton()->GetTime();
        /*int dim = 3;
        int refNum = pRef->GetPointNumber();
        float* dataSet = new float[refNum * dim];
        for (int i = 0; i < refNum; i++)
        {
            Vector3 pos = pRef->GetPoint(i)->GetPosition();
            dataSet[dim * i + 0] = pos[0];
            dataSet[dim * i + 1] = pos[1];
            dataSet[dim * i + 2] = pos[2];
        }*/
        
        /*FLANNParameters searchPara;
        searchPara = DEFAULT_FLANN_PARAMETERS;
        searchPara.algorithm = FLANN_INDEX_KDTREE;
        searchPara.trees = 8;
        searchPara.log_level = FLANN_LOG_INFO;
	    searchPara.checks = 64;
        float speedup;
        flann_index_t indexId = flann_build_index(dataSet, refNum, dim, &speedup, &searchPara);
        MagicLog << "        Flann: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeStart << std::endl;*/
        int nn = 1;
        int dim = 3;
        int searchNum = sampleIndex.size();
        float* searchSet = new float[searchNum * dim];
        for (int i = 0; i < searchNum; i++)
        {
            Vector3 pos = pTransInit->TransformPoint( pOrigin->GetPoint(sampleIndex.at(i))->GetPosition() );
            searchSet[dim * i + 0] = pos[0];
            searchSet[dim * i + 1] = pos[1];
            searchSet[dim * i + 2] = pos[2];
        }
        int* pIndex = new int[searchNum * nn];
        float* pDist = new float[searchNum * nn];
        flann_find_nearest_neighbors_index(mFlannIndex, searchSet, searchNum, pIndex, pDist, nn, &mSearchPara);
        //flann_free_index(indexId, &searchPara);
        //delete []dataSet;
        delete []searchSet;
        

        //delete wrong correspondance
        float distThre = 500.f;
        float norThre = 0.1f; //cos(85);
        std::vector<int> sampleIndexBak = sampleIndex;
        sampleIndex.clear();
        correspondIndex.clear();
        for (int i = 0; i < searchNum; i++)
        {
            if (pDist[i] > distThre)
            {
        //        MagicLog << "Large dist: " << pDist[i] << std::endl;
                continue;
            }
            float norDist = pRef->GetPoint(pIndex[i])->GetNormal() * (pTransInit->RotateVector( pOrigin->GetPoint(sampleIndexBak.at(i))->GetNormal() ));
            if (norDist < norThre || norDist > 1.0)
            {
         //       MagicLog << "Large Nor: " << norDist << std::endl;
                continue;
            }
            sampleIndex.push_back(sampleIndexBak.at(i));
            correspondIndex.push_back(pIndex[i]);
        }
        //MagicLog << "Sample Number: " << sampleIndex.size() << std::endl;
        delete []pIndex;
        delete []pDist;
    }

    void Registration::ICPEnergyMinimization(const Point3DSet* pRef, const Point3DSet* pOrigin, const HomoMatrix4* pTransInit, 
            std::vector<int>& sampleIndex, std::vector<int>& correspondIndex, HomoMatrix4* pTransDelta)
    {
        //MagicLog << "Registration::ICPEnergyMinimization" << std::endl;
        int pcNum = sampleIndex.size();
        Eigen::MatrixXd matA(pcNum, 6);
        Eigen::VectorXd vecB(pcNum, 1);
        for (int i = 0; i < pcNum; i++)
        {
            Vector3 norRef = pRef->GetPoint(correspondIndex.at(i))->GetNormal();
            Vector3 posRef = pRef->GetPoint(correspondIndex.at(i))->GetPosition();
            Vector3 posPC  = pTransInit->TransformPoint( pOrigin->GetPoint(sampleIndex.at(i))->GetPosition() );
            vecB(i) = (posRef - posPC) * norRef;
            Vector3 coffTemp = posPC.CrossProduct(norRef);
            matA(i, 0) = coffTemp[0];
            matA(i, 1) = coffTemp[1];
            matA(i, 2) = coffTemp[2];
            matA(i, 3) = norRef[0];
            matA(i, 4) = norRef[1];
            matA(i, 5) = norRef[2];
        }
        Eigen::MatrixXd matAT = matA.transpose();
        Eigen::MatrixXd matCoefA = matAT * matA;
        Eigen::MatrixXd vecCoefB = matAT * vecB;
        Eigen::VectorXd res = matCoefA.ldlt().solve(vecCoefB);
        pTransDelta->Unit();
        pTransDelta->SetValue(0, 1, -res(2));
        pTransDelta->SetValue(0, 2, res(1));
        pTransDelta->SetValue(0, 3, res(3));
        pTransDelta->SetValue(1, 0, res(2));
        pTransDelta->SetValue(1, 2, -res(0));
        pTransDelta->SetValue(1, 3, res(4));
        pTransDelta->SetValue(2, 0, -res(1));
        pTransDelta->SetValue(2, 1, res(0));
        pTransDelta->SetValue(2, 3, res(5));
        //print result
        /*MagicLog << "MatA: " << std::endl;
        MagicLog << 1 << " " << -res(2) << " " << res(1) << " " << res(3) << std::endl;
        MagicLog << res(2) << " " << 1 << " " << -res(0) << " " << res(4) << std::endl;
        MagicLog << -res(1) << " " << res(0) << " " << 1 << " " << res(5) << std::endl;*/
    }

    void Registration::ICPRegistrateEnhance(const Point3DSet* pRefPC, Point3DSet* pNewPC, const HomoMatrix4* pTransInit, HomoMatrix4* pTransRes, openni::VideoStream& depthStream)
    {
        //int iterNum = 10;
        //*pTransRes = *pTransInit;
        ////ICPInitRefDataEnhance(pNewPC);
        //std::vector<int> sampleIndex;
        //ICPSamplePointEnhance(pRefPC, sampleIndex, pTransRes);
        //for (int k = 0; k < iterNum; k++)
        //{
        //    //float timeCorres = MagicCore::ToolKit::GetSingleton()->GetTime();
        //    std::vector<int> correspondIndex;
        //    ICPFindCorrespondanceEnhance(pRefPC, pNewPC, pTransRes, sampleIndex, correspondIndex);
        //    //MagicLog << "        ICPCorres: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeCorres << std::endl;
        //    //float timeMinimize = MagicCore::ToolKit::GetSingleton()->GetTime();
        //    HomoMatrix4 transDelta;
        //    ICPEnergyMinimizationEnhance(pRefPC, pNewPC, pTransRes, sampleIndex, correspondIndex, &transDelta);
        //    //MagicLog << "        ICPMinimize: " << MagicCore::ToolKit::GetSingleton()->GetTime() - timeMinimize << std::endl;
        //    //*pTransRes *= transDelta;
        //    *pTransRes = transDelta * (*pTransRes);
        //    //MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("newPC", "SimplePoint_Green", pOrigin, *pTransRes);
        //    //MagicCore::RenderSystem::GetSingleton()->Update();
        //    if (fabs(transDelta.GetValue(0, 3)) < 0.01f && 
        //        fabs(transDelta.GetValue(1, 3)) < 0.01f && 
        //        fabs(transDelta.GetValue(2, 3)) < 0.01f)
        //    {
        //        MagicLog << "ICP iterator number: " << k + 1 << std::endl;
        //        break;
        //    }
        //}
    }

    //void Registration::ICPInitRefDataEnhance(const Point3DSet* pNewPC)
    //{

    //}

    void Registration::ICPSamplePointEnhance(const Point3DSet* pPC, std::vector<int>& sampleIndex, const HomoMatrix4* pTransform, openni::VideoStream& depthStream)
    {
        std::vector<openni::DepthPixel> depthCache(640 * 480, 0);
        int pcNum = pPC->GetPointNumber();
        for (int i = 0; i < pcNum; i++)
        {
            Vector3 pos = pPC->GetPoint(i)->GetPosition();
            int depthX, depthY;
            openni::DepthPixel depthZ;
            openni::CoordinateConverter::convertWorldToDepth(depthStream, pos[0], pos[1], pos[2], &depthX, &depthY, &depthZ);
            if (depthCache.at(depthX * 480 + depthY) == 0)
            {
                depthCache.at(depthX * 480 + depthY) = depthZ;
            }
            else if (depthCache.at(depthX * 480 + depthY) > depthZ)
            {
                depthCache.at(depthX * 480 + depthY) = depthZ;
            }
        }
        for (int i = 0; i < pcNum; i++)
        {
            Vector3 pos = pPC->GetPoint(i)->GetPosition();
            int depthX, depthY;
            openni::DepthPixel depthZ;
            openni::CoordinateConverter::convertWorldToDepth(depthStream, pos[0], pos[1], pos[2], &depthX, &depthY, &depthZ);
            if (depthCache.at(depthX * 480 + depthY) == depthZ)
            {
                sampleIndex.push_back(i);
            }
        }
    }

    void Registration::ICPFindCorrespondanceEnhance(const Point3DSet* pRefPC, const Point3DSet* pNewPC, const HomoMatrix4* pTransInit,
            std::vector<int>& sampleIndex,  std::vector<int>& correspondIndex, openni::VideoStream& depthStream)
    {

    }

    void Registration::ICPEnergyMinimizationEnhance(const Point3DSet* pRefPC, const Point3DSet* pNewPC, const HomoMatrix4* pTransInit,
            std::vector<int>& sampleIndex, std::vector<int>& correspondIndex, HomoMatrix4* pTransDelta)
    {

    }
}

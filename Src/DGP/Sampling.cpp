#include "Sampling.h"
#include "../Common/LogSystem.h"

namespace MagicDGP
{
    Sampling::Sampling()
    {
    }

    Sampling::~Sampling()
    {
    }

    Point3DSet* Sampling::WLOPSampling(const Point3DSet* pPS, int sampleNum)
    {
        MagicLog << "Begin Sampling::WLOPSampling" << std::endl;
        std::vector<Vector3> samplePosList;
        InitialSampling(pPS, sampleNum, samplePosList);
        sampleNum = samplePosList.size();
        MagicLog << "Finsh Initial Sampling: " << sampleNum << std::endl;
        int iteratNum = 0;
        for (int i = 0; i < iteratNum; i++)
        {
            WLOPIteration(pPS, samplePosList);
        }
        MagicLog << "Finish WLOP Iteration" << std::endl;
        std::vector<Vector3> norList;
        LocalPCANormalEstimate(samplePosList, norList);
        MagicLog << "Finish Normal Estimate" << std::endl;
        Point3DSet* pNewPS = new Point3DSet;
        for (int i = 0; i < sampleNum; i++)
        {
            Point3D* pPoint = new Point3D(samplePosList.at(i), norList.at(i));
            pNewPS->InsertPoint(pPoint);
        }
        return pNewPS;
    }

    void Sampling::InitialSampling(const Point3DSet* pPS, int sampleNum, std::vector<Vector3>& samplePosList)
    {
        int psNum = pPS->GetPointNumber();
        int delta = psNum / sampleNum;
        if (delta < 1)
        {
            MagicLog << "Sample number less than point set number" << std::endl;
            delta = 1;
            sampleNum = psNum;
        }
        samplePosList.clear();
        samplePosList.reserve(sampleNum);
        for (int i = 0; i < psNum; i += delta)
        {
            samplePosList.push_back(pPS->GetPoint(i)->GetPosition());
        }
    }

    void Sampling::WLOPIteration(const Point3DSet* pPS, std::vector<Vector3> & samplePosList)
    {
        MagicLog << "Begin Sampling::WLOPIteration" << std::endl;
        int iNum = samplePosList.size();
        int jNum = pPS->GetPointNumber();
        Real psBBSize = pPS->GetBBSize();
        //Real thetaScale = -jNum / psBBSize;
        Real thetaScale = -1000;
        Real smallValue = 1.0e-10;
        Real mu = 0.45;
        std::vector<Real> alpha(jNum);
        std::vector<Real> beta(iNum);
        std::vector<Vector3> samplePosBak = samplePosList;
        MagicLog << "BBSize: " << psBBSize << " thetaScale" << thetaScale << std::endl;
        for (int i = 0; i < iNum; i++)
        {
            for (int j = 0; j < jNum; j++)
            {
                Vector3 deltaPos = samplePosBak.at(i) - pPS->GetPoint(j)->GetPosition();
                Real deltaLen = deltaPos.Length();
                if (deltaLen < smallValue)
                {
                    //MagicLog << "Alpha: small deltaLen: " << deltaLen;
                    deltaLen = smallValue;
                    //MagicLog << " " << exp(thetaScale * deltaLen * deltaLen) / deltaLen << std::endl; 
                }
                alpha.at(j) = exp(thetaScale * deltaLen * deltaLen) / deltaLen;
            }
            for (int j = 0; j < iNum; j++)
            {
                Vector3 deltaPos = samplePosBak.at(i) - samplePosBak.at(j); //pPS->GetPoint(i)->GetPosition() - pPS->GetPoint(j)->GetPosition();
                Real deltaLen = deltaPos.Length();
                if (deltaLen < smallValue)
                {
                    deltaLen = smallValue;
                }
                beta.at(j) = -exp(thetaScale * deltaLen * deltaLen) / deltaLen;
            }
            Vector3 posRes1(0, 0, 0);
            Real alphaSum = 0;
            for (int j = 0; j < jNum; j++)
            {
                posRes1 += pPS->GetPoint(j)->GetPosition() * alpha.at(j);
                alphaSum += alpha.at(j);
            }
            if (alphaSum < smallValue)
            {
                MagicLog << "Small AlphaSum: " << alphaSum << std::endl;
                alphaSum = smallValue;
            }
            posRes1 /= alphaSum;

            Vector3 posRes2(0, 0, 0);
            Real betaSum = 0;
            Vector3 posI = samplePosBak.at(i); 
            for (int ii = 0; ii < iNum; ii++)
            {
                if (ii != i)
                {
                    posRes2 += (posI - samplePosBak.at(ii)) * beta.at(ii);
                    betaSum += beta.at(ii);
                }
            }
            if (fabs(betaSum) < smallValue)
            {
                MagicLog << "Small betaSum: " << betaSum << std::endl;
                betaSum = -smallValue;
            }
            posRes2 /= betaSum;
            samplePosList.at(i) = posRes1 + posRes2 * mu;
        }
    }

    void Sampling::LocalPCANormalEstimate(const std::vector<Vector3>& samplePosList, std::vector<Vector3>& norList)
    {
        norList.clear();
        for (int i = 0; i < samplePosList.size(); i++)
        {
            norList.push_back(Vector3(0, 0, 1));
        }
    }
}
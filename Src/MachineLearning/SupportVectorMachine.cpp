#include "SupportVectorMachine.h"
#include "KernelFunction.h"
#include "../Tool/ErrorCodes.h"
#include "../Tool/LogSystem.h"
#include <set>

namespace MagicML
{
    SupportVectorMachine::SupportVectorMachine() :
        mpKernel(NULL),
        mB(0),
        mAlpha(),
        mSupportVecX(),
        mSupportVecY()
    {
    }

    SupportVectorMachine::~SupportVectorMachine()
    {
        if (mpKernel != NULL)
        {
            delete mpKernel;
            mpKernel = NULL;
        }
    }

    void SupportVectorMachine::Reset(void)
    {
        if (mpKernel != NULL)
        {
            delete mpKernel;
            mpKernel = NULL;
        }
        mB = 0;
        mAlpha.clear();
        mSupportVecX.clear();
        mSupportVecY.clear();
    }

    void SupportVectorMachine::Learn(const std::vector<double>& dataX, const std::vector<double>& dataY, 
        KernelFunction* kernel, double softCoef)
    {
        Reset();
        mpKernel = kernel;
        SequentialMinimalOptimization(dataX, dataY, softCoef);
    }

    double SupportVectorMachine::CalF(int index) const // Could be optimized
    {
        double res = 0;
        int dataDim = mSupportVecX.size() / mSupportVecY.size();
        for (int i = 0; i < mAlpha.size(); i++)
        {
            res += mAlpha.at(i) * mSupportVecY.at(i) * mpKernel->InnerProduct(mSupportVecX, index, i, dataDim);
        }
        res -= mB;
        return res;
    }

    bool SupportVectorMachine::IsKTT(int index, double softCoef) const
    {
        double F = CalF(index);
        if ((mSupportVecY.at(index) * F < 1 && mAlpha.at(index) < softCoef) ||
            (mSupportVecY.at(index) * F > 1 && mAlpha.at(index) > 0))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    int SupportVectorMachine::ChooseIndexJ(int index_i, const std::set<int>& nonBoundSet) const
    {
        int index_j = -1;
        double maxV = -1;
        double E_i = CalF(index_i) - mSupportVecY.at(index_i);
        for (std::set<int>::iterator itr = nonBoundSet.begin(); itr != nonBoundSet.end(); itr++)
        {
            double E_j = CalF(*itr) - mSupportVecY.at(*itr);
            if (fabs(E_i - E_j) > maxV)
            {
                maxV = fabs(E_i - E_j);
                index_j = *itr;
            }
        }
        if (index_j == -1)
        {
            for (int jDataid = 0; jDataid < mSupportVecY.size(); jDataid++)
            {
                if (jDataid == index_i)
                {
                    continue;
                }
                double E_j = CalF(jDataid) - mSupportVecY.at(jDataid);
                if (fabs(E_i - E_j) > maxV)
                {
                    maxV = fabs(E_i - E_j);
                    index_j = jDataid;
                }
            }
        }
        return index_j;
    }

    void SupportVectorMachine::SequentialMinimalOptimization(const std::vector<double>& dataX, const std::vector<double>& dataY, double softCoef)
    {
        //Initialization
        mSupportVecX = dataX;
        mSupportVecY = dataY;
        int dataCount = mSupportVecY.size();
        int dataDim = mSupportVecX.size() / mSupportVecY.size();
        mAlpha.clear();
        mAlpha = std::vector<double>(dataCount, 0.0);
        mB = 0.0;
        double localEpsilon = 0.01;

        int maxIterCount = 1000;
        std::set<int> kktList;
        for (int iterId = 0; iterId < maxIterCount; iterId++)
        {
            //first pass all variables
            std::set<int> nonBoundSet;
            for (int dataId = 0; dataId < dataCount; dataId++)
            {
                //choose index_i
                int index_i = -1;
                if (IsKTT(dataId, softCoef))
                {
                    continue;
                }
                else
                {
                    index_i = dataId;
                }
                //choose index_j
                int index_j = ChooseIndexJ(index_i, nonBoundSet);
                if (index_j == -1)
                {
                    continue;
                }
                OptimizeOneStep(index_i, index_j, softCoef);
                //Update nonBoundSet
                if (mAlpha.at(index_i) > localEpsilon && mAlpha.at(index_i) < softCoef - localEpsilon)
                {
                    nonBoundSet.insert(index_i);
                }
                if (mAlpha.at(index_j) > localEpsilon && mAlpha.at(index_j) < softCoef - localEpsilon)
                {
                    nonBoundSet.insert(index_j);
                }
            } //first pass

            //second pass nonBoundSet
            int maxNonBoundIterNum = 100;
            for (int nonBoundIterId = 0; nonBoundIterId < maxNonBoundIterNum; nonBoundIterId++)
            {
                if (nonBoundSet.size() == 0)
                {
                    break;
                }
                for (std::set<int>::iterator itr = nonBoundSet.begin(); itr != nonBoundSet.end(); itr++)
                {
                    //choose index_i
                    int index_i = -1;
                    if (IsKTT(*itr, softCoef))
                    {
                        continue;
                    }
                    else
                    {
                        index_i = *itr;
                    }
                    //choose index_j
                    int index_j = ChooseIndexJ(index_i, nonBoundSet);
                    if (index_j == -1)
                    {
                        continue;
                    }
                    OptimizeOneStep(index_i, index_j, softCoef);
                    //Update nonBoundSet
                    if (mAlpha.at(index_i) > localEpsilon && mAlpha.at(index_i) < softCoef - localEpsilon)
                    {
                        nonBoundSet.insert(index_i);
                    }
                    if (mAlpha.at(index_j) > localEpsilon && mAlpha.at(index_j) < softCoef - localEpsilon)
                    {
                        nonBoundSet.insert(index_j);
                    }
                } //nonBound pass
            } //second pass
        }
        DebugLog << "SupportVectorMachine::SequentialMinimalOptimization finished" << std::endl;
    }

    int SupportVectorMachine::OptimizeOneStep(int index_i, int index_j, double softCoef)
    {
        int dataDim = mSupportVecX.size() / mSupportVecY.size();
        //Update alpha i and j
        double lastAlpha_i = mAlpha.at(index_i);
        double lastAlpha_j = mAlpha.at(index_j);
        double L, H;
        if (mSupportVecY.at(index_i) * mSupportVecY.at(index_j) < 0)
        {
            double deltaTemp = mAlpha.at(index_j) - mAlpha.at(index_i);
            L = deltaTemp > 0 ? deltaTemp : 0;
            H = deltaTemp > 0 ? softCoef : softCoef + deltaTemp;
        }
        else
        {
            double deltaTemp = mAlpha.at(index_j) + mAlpha.at(index_i);
            L = deltaTemp > softCoef ? deltaTemp - softCoef : 0;
            H = deltaTemp > softCoef ? softCoef : deltaTemp;
        }

        double eta = mpKernel->InnerProduct(mSupportVecX, index_i, index_i, dataDim) + mpKernel->InnerProduct(mSupportVecX, index_j, index_j, dataDim) 
            - 2 * mpKernel->InnerProduct(mSupportVecX, index_i, index_j, dataDim);
        if (fabs(eta) < 1.0e-15)
        {
            eta += 1.0e-15;
        }
        double E_i = CalF(index_i) - mSupportVecY.at(index_i);
        double E_j = CalF(index_j) - mSupportVecY.at(index_j);
        mAlpha.at(index_j) += mSupportVecY.at(index_j) * (E_i - E_j) / eta;
        if (mAlpha.at(index_j) < L)
        {
            mAlpha.at(index_j) = L;
        }
        else if (mAlpha.at(index_j) > H)
        {
            mAlpha.at(index_j) = H;
        }
        mAlpha.at(index_i) += mSupportVecY.at(index_i) * mSupportVecY.at(index_j) * (lastAlpha_j - mAlpha.at(index_j));
        //Update b
        if (mAlpha.at(index_i) > 0 && mAlpha.at(index_i) < softCoef)
        {
            mB = mB + E_i + mSupportVecY.at(index_i) * (mAlpha.at(index_i) - lastAlpha_i) * mpKernel->InnerProduct(mSupportVecX, index_i, index_i, dataDim) 
                + mSupportVecY.at(index_j) * (mAlpha.at(index_j) - lastAlpha_j) * mpKernel->InnerProduct(mSupportVecX, index_i, index_j, dataDim);
        }
        else if (mAlpha.at(index_j) > 0 && mAlpha.at(index_j) < softCoef)
        {
            mB = mB + E_j + mSupportVecY.at(index_i) * (mAlpha.at(index_i) - lastAlpha_i) * mpKernel->InnerProduct(mSupportVecX, index_i, index_j, dataDim) 
                + mSupportVecY.at(index_j) * (mAlpha.at(index_j) - lastAlpha_j) * mpKernel->InnerProduct(mSupportVecX, index_j, index_j, dataDim);
        }
        else
        {
            double b1 = mB + E_i + mSupportVecY.at(index_i) * (mAlpha.at(index_i) - lastAlpha_i) * mpKernel->InnerProduct(mSupportVecX, index_i, index_i, dataDim) 
                + mSupportVecY.at(index_j) * (mAlpha.at(index_j) - lastAlpha_j) * mpKernel->InnerProduct(mSupportVecX, index_i, index_j, dataDim);
            double b2 = mB + E_j + mSupportVecY.at(index_i) * (mAlpha.at(index_i) - lastAlpha_i) * mpKernel->InnerProduct(mSupportVecX, index_i, index_j, dataDim) 
                + mSupportVecY.at(index_j) * (mAlpha.at(index_j) - lastAlpha_j) * mpKernel->InnerProduct(mSupportVecX, index_j, index_j, dataDim);
            mB = (b1 + b2) / 2;
        }
        return 0;
    }
        
    double SupportVectorMachine::Predict(const std::vector<double>& dataX) const
    {
        int supCount = mAlpha.size();
        double res = 0;
        int dataDim = mSupportVecX.size() / mSupportVecY.size();
        for (int supId = 0; supId < supCount; supId++)
        {
            res += mAlpha.at(supId) * mSupportVecY.at(supId) * mpKernel->InnerProduct(mSupportVecX, supId, dataX);
        }
        res += mB;

        return res;
    }
}

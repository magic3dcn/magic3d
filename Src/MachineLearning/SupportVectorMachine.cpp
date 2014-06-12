#include "SupportVectorMachine.h"
#include "KernelFunction.h"
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

    double SupportVectorMachine::CalU(int index, const std::vector<double>& dataX, const std::vector<double>& dataY)
    {
        double res = 0;
        int dataDim = dataX.size() / dataY.size();
        for (int i = 0; i < mAlpha.size(); i++)
        {
            res += mAlpha.at(i) * dataY.at(i) * mpKernel->InnerProduct(dataX, index, i, dataDim);
        }
        return res;
    }

    void SupportVectorMachine::SequentialMinimalOptimization(const std::vector<double>& dataX, const std::vector<double>& dataY, double softCoef)
    {
        int dataCount = dataY.size();
        int dataDim = dataX.size() / dataY.size();
        mAlpha.clear();
        mAlpha = std::vector<double>(dataCount, 0.0);
        mB = 0.0;
        int maxIterCount = 10;
        std::set<int> kktList;
        for (int iterId = 0; iterId < maxIterCount; iterId++)
        {
            //Choose i and j, calculate Ei and Ej
            double E_i = 0;
            int index_i = -1;
            for (int dataId = 0; dataId < dataCount; dataId++)
            {
                double U_i = CalU(dataId, dataX, dataY);
                if ((dataY.at(dataId) * U_i < 1 && mAlpha.at(dataId) < softCoef) || 
                    (dataY.at(dataId) * U_i > 1 && mAlpha.at(dataId) > 0))
                {
                    index_i = dataId;
                    E_i = U_i - dataY.at(dataId);
                }
            }
            if (index_i == -1)
            {
                break;
            }
            int index_j = -1;
            double E_j = 0;
            if (kktList.size() > 0)
            {
                double maxV = -1;
                for (std::set<int>::iterator itr = kktList.begin(); itr != kktList.end(); itr++)
                {
                    E_j = CalU(*itr, dataX, dataY) - dataY.at(*itr);
                    if (fabs(E_i - E_j) > maxV)
                    {
                        maxV = fabs(E_i - E_j);
                        index_j = *itr;
                    }
                }
            }
            else
            {
                index_j = (index_i + iterId) % dataCount; //random number
                E_j = CalU(index_j, dataX, dataY) - dataY.at(index_j);
            }
            //Update alpha i and j
            double lastAlpha_i = mAlpha.at(index_i);
            double lastAlpha_j = mAlpha.at(index_j);
            double L, H;
            if (dataY.at(index_i) * dataY.at(index_j) < 0)
            {
                double deltaTemp = mAlpha.at(index_j) - mAlpha.at(index_i);
                L = deltaTemp > 0 ? deltaTemp : 0;
                H = deltaTemp > 0 ? softCoef : softCoef + deltaTemp;
            }
            else
            {
                double deltaTemp = mAlpha.at(index_j) + mAlpha.at(index_i);
                L = deltaTemp > softCoef ? deltaTemp - softCoef : 0;
                H = deltaTemp > 0 ? 0 : deltaTemp;
            }

            double eta = mpKernel->InnerProduct(dataX, index_i, index_i, dataDim) + mpKernel->InnerProduct(dataX, index_j, index_j, dataDim) 
                - 2 * mpKernel->InnerProduct(dataX, index_i, index_j, dataDim);
            if (fabs(eta) < 1.0e-15)
            {
                break;
            }
            mAlpha.at(index_j) += dataY.at(index_j) * (E_i - E_j) / eta;
            if (mAlpha.at(index_j) > 0 && mAlpha.at(index_j) < softCoef)
            {
                kktList.insert(index_j);
            }
            if (mAlpha.at(index_j) < L)
            {
                mAlpha.at(index_j) = L;
            }
            else if (mAlpha.at(index_j) > H)
            {
                mAlpha.at(index_j) = H;
            }
            mAlpha.at(index_i) += dataY.at(index_i) * dataY.at(index_j) * (lastAlpha_j - mAlpha.at(index_j));
            if (mAlpha.at(index_i) > 0 && mAlpha.at(index_i) < softCoef)
            {
                kktList.insert(index_i);
            }
            //Update b
            double b1 = mB - E_i - dataY.at(index_i) * (mAlpha.at(index_i) - lastAlpha_i) * mpKernel->InnerProduct(dataX, index_i, index_i, dataDim) 
                - dataY.at(index_j) * (mAlpha.at(index_j) - lastAlpha_j) * mpKernel->InnerProduct(dataX, index_i, index_j, dataDim);
            double b2 = mB - E_j - dataY.at(index_i) * (mAlpha.at(index_i) - lastAlpha_i) * mpKernel->InnerProduct(dataX, index_i, index_j, dataDim) 
                - dataY.at(index_j) * (mAlpha.at(index_j) - lastAlpha_j) * mpKernel->InnerProduct(dataX, index_j, index_j, dataDim);
            if (mAlpha.at(index_i) > 0 && mAlpha.at(index_i) < softCoef)
            {
                mB = b1;
            }
            else if (mAlpha.at(index_j) > 0 && mAlpha.at(index_j) < softCoef)
            {
                mB = b2;
            }
            else
            {
                mB = (b1 + b2) / 2;
            }
        }
    }
        
    double SupportVectorMachine::Predict(const std::vector<double>& dataX) const
    {
        int supCount = mAlpha.size();
        double res = 0;
        int dataDim = mSupportVecX.size() / mSupportVecY.size();
        for (int supId = 0; supId < supCount; supId++)
        {
            res += mAlpha.at(supId) * mSupportVecY.at(supId) * mpKernel->InnerProduct(mSupportVecX, supId * dataDim, dataX);
        }
        res += mB;

        return res;
    }
}

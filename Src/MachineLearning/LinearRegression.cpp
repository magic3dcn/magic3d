#include "LinearRegression.h"

namespace MagicML
{
    LinearRegression::LinearRegression() :
        mInputDim(0),
        mOutputDim(0),
        mRegMat()
    {
    }

    LinearRegression::~LinearRegression()
    {
    }

    void LinearRegression::Learn(const std::vector<double>& input, const std::vector<double>& output, int dataSize)
    {
        Reset();
        mInputDim = input.size() / dataSize;
        mOutputDim = output.size() / dataSize;

    }

    std::vector<double> LinearRegression::Predict(const std::vector<double>& input)
    {
        std::vector<double> output(mOutputDim);
        for (int outDim = 0; outDim < mOutputDim; outDim++)
        {
            double tempV = 0;
            int baseIndex = outDim * mInputDim;
            for (int inDim = 0; inDim < mInputDim; inDim++)
            {
                tempV += (input.at(inDim) * mRegMat.at(baseIndex + inDim));
            }
            tempV += mRegMat.at(baseIndex + mInputDim);
            output.at(outDim) = tempV;
        }
        return output;
    }

    int LinearRegression::GetInputDim()
    {
        return mInputDim;
    }

    int LinearRegression::GetOutputDim()
    {
        return mOutputDim;
    }

    void LinearRegression::Reset()
    {
        mInputDim = 0;
        mOutputDim = 0;
        mRegMat.clear();
    }
}

#include "Optimization.h"

namespace MagicMath
{
    GradientDescent::GradientDescent() :
        mMaxIterCount(10000000)
    {
    }

    GradientDescent::GradientDescent(int maxIterCount) :
        mMaxIterCount(maxIterCount)
    {
    }

    GradientDescent::~GradientDescent()
    {
    }

    void GradientDescent::Run()
    {
        CalInitValue();
        for (int iterId = 0; iterId < mMaxIterCount; iterId++)
        {
            CalGradient();
            if (IsStop())
            {
                break;
            }
            CalStepLength();
            UpdateResult();
        }
    }
}

#pragma once
#include <vector>
#include <set>

namespace MagicML
{
    class KernelFunction;
    class SupportVectorMachine
    {
    public:
        SupportVectorMachine();
        ~SupportVectorMachine();

        void Learn(const std::vector<double>& dataX, const std::vector<double>& dataY, KernelFunction* kernel, double softCoef);
        double Predict(const std::vector<double>& dataX) const;

    private:
        void Reset(void);
        void SequentialMinimalOptimization(const std::vector<double>& dataX, const std::vector<double>& dataY, double softCoef);
        double CalF(int index) const;
        int OptimizeOneStep(int index_i, int index_j, double softCoef);
        bool IsKTT(int index, double softCoef) const;
        int ChooseIndexJ(int index_i, const std::set<int>& nonBoundSet) const;

    private:
        KernelFunction* mpKernel;
        double mB;
        std::vector<double> mAlpha;
        std::vector<double> mSupportVecX;
        std::vector<double> mSupportVecY;
    };
}

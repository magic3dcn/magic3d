#pragma once
#include <vector>

namespace MagicML
{
    class KernelFunction
    {
    public:
        KernelFunction();
        virtual ~KernelFunction() = 0;
        virtual double InnerProduct(const std::vector<double>& vec1, const std::vector<double>& vec2) = 0;
    };



    class SupportVectorMachine
    {
    public:
        SupportVectorMachine();
        ~SupportVectorMachine();

        void Learn(const std::vector<double>& dataX, const std::vector<double>& dataY, const KernelFunction& kernel, double softCoef);
        double Predict(const std::vector<double>& dataX) const;
    };
}

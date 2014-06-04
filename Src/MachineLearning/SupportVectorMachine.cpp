#include "SupportVectorMachine.h"

namespace MagicML
{

    KernelFunction::KernelFunction()
    {
    }

    SupportVectorMachine::SupportVectorMachine()
    {
    }

    SupportVectorMachine::~SupportVectorMachine()
    {
    }

    void SupportVectorMachine::Learn(const std::vector<double>& dataX, const std::vector<double>& dataY, 
        const KernelFunction& kernel, double softCoef)
    {

    }
        
    double SupportVectorMachine::Predict(const std::vector<double>& dataX) const
    {
        return 0;
    }
}

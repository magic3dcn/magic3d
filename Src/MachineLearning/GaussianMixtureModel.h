#pragma once
#include <vector>

namespace MagicML
{
    class GaussianMixtureModel
    {
    public:
        GaussianMixtureModel(const std::vector<double>& inputData, int dim, int k);
        ~GaussianMixtureModel();

        double Pro(const std::vector<double>& data);
        double Pro(const std::vector<double>& data, int componentId);

    private:

    };

}

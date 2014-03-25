#pragma once
#include <vector>

namespace MagicML
{
    class Clustering
    {
    public:
        Clustering();
        ~Clustering();

        static void OrchardBoumanClustering(const std::vector<double>& inputData, int dim, int k, std::vector<int>& clusterRes);

    private:
        static void CalEigenVector(const std::vector<double>& inputData, int dim, const std::vector<int>& inputIndex, 
            std::vector<double>& eigenVector, double& eigenValue, std::vector<double>& meanVector);
        static void SplitSetByEigenVector(const std::vector<double>& inputData, int dim, const std::vector<int>& inputIndex,
            const std::vector<double>& eigenVector, const std::vector<double>& meanVector,
            std::vector<int>& clusterA, std::vector<int>& clusterB);
    };

}
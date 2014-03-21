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

    };

}
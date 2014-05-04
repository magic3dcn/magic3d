#pragma once
#include <vector>

namespace MagicML
{
    class PrincipalComponentAnalysis
    {
    public:
        PrincipalComponentAnalysis();
        ~PrincipalComponentAnalysis();

        void Analyse(const std::vector<double>& data, int dataDim, int pcaDim);
        std::vector<double> GetEigenVector(int k);
        double GetEigenValue(int k);
        std::vector<double> GetAvgVector(void);
        std::vector<double> Project(const std::vector<double>& data);
        void Load(void);
        void Save(void);

    private:
        void Clear(void);

    private:
        int mDataDim;
        int mPcaDim;
        std::vector<double> mEigenVectors;
        std::vector<double> mEigenValues;
        std::vector<double> mAvgVector;
    };
}

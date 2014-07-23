#include "RealTimeFaceDetection.h"
#include "../Tool/ErrorCodes.h"
#include "../Tool/LogSystem.h"

namespace MagicDIP
{

    HaarClassifier::HaarClassifier()
    {
    }

    HaarClassifier::~HaarClassifier()
    {
    }

    int HaarClassifier::Learn(const ImageLoader& imgLoader, const std::vector<double>& dataWeights, const std::vector<int>& dataY, 
            double* trainError)
    {
        return MAGIC_NO_ERROR;
    }
     
    int HaarClassifier::Predict(const cv::Mat& img, int sRow, int sCol, float scale) const
    {
        return 0;
    }

    int HaarClassifier::Predict(const ImageLoader& imgLoader, int dataId) const
    {
        return 0;
    }

    AdaBoostFaceDetection::AdaBoostFaceDetection()
    {
    }

    AdaBoostFaceDetection::~AdaBoostFaceDetection()
    {
    }

    int AdaBoostFaceDetection::Learn(const ImageLoader& imgLoader, const std::vector<int>& dataY, int levelCount)
    {
        Reset();
        mClassifiers.reserve(levelCount);
        mClassifierWeights.reserve(levelCount);
        int dataCount = dataY.size();
        std::vector<double> dataWeights(dataCount, 1.0 / dataCount);
        std::vector<int> resFlag(dataCount);
        double epsilon = 1.0e-5;
        mThreshold = 0.0;
        for (int levelId = 0; levelId < levelCount; levelId++)
        {
            HaarClassifier* pWeakClassifier = TrainWeakClassifier(imgLoader, dataWeights, dataY);
            if (pWeakClassifier != NULL)
            {
                mClassifiers.push_back(pWeakClassifier);
            }
            else
            {
                Reset();
                DebugLog << "Error: NULL Weak Classifier" << std::endl;
                return MAGIC_INVALID_RESULT;
            }
            double trainingError = 0.0;
            for (int dataId = 0; dataId < dataCount; dataId++)
            {
                resFlag.at(dataId) = abs(dataY.at(dataId) - pWeakClassifier->Predict(imgLoader, dataId));
                trainingError += dataWeights.at(dataId) * resFlag.at(dataId);
            }
            if (trainingError < 0.5)
            {
                DebugLog << "Weak Classifier " << levelId << ": " << trainingError << std::endl;
            }

            if (fabs(trainingError - 1.0) < epsilon)
            {
                trainingError = 1.0 - epsilon;
                DebugLog << "Weak Classifier " << levelId << ": is too weak!" << std::endl;
            }
            else if (fabs(trainingError) < epsilon)
            {
                trainingError = epsilon;
                DebugLog << "Weak Classifier " << levelId << ": is too good!" << std::endl;
            }

            double beta = trainingError / (1.0 - trainingError);
            double weight = log(1.0 / beta);
            mClassifierWeights.push_back(weight);
            mThreshold += weight;

            //Update dataWeights
            double weightSum = 0.0;
            for (int dataId = 0; dataId < dataCount; dataId++)
            {
                dataWeights.at(dataId) *= pow(beta, resFlag.at(dataId));
                weightSum += dataWeights.at(dataId);
            }
            for (std::vector<double>::iterator itr = dataWeights.begin(); itr != dataWeights.end(); itr++)
            {
                *itr /= weightSum;
            }
        }
        mThreshold *= 0.5;

        return MAGIC_NO_ERROR;
    }
    
    int AdaBoostFaceDetection::Predict(const cv::Mat& img, int sRow, int sCol, float scale) const
    {
        double res = 0.0;
        int classifierCount = mClassifierWeights.size();
        for (int cid = 0; cid < classifierCount; cid++)
        {
            res += mClassifiers.at(cid)->Predict(img, sRow, sCol, scale) * mClassifierWeights.at(cid);
        }
        if (res > mThreshold)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    HaarClassifier* AdaBoostFaceDetection::TrainWeakClassifier(const ImageLoader& imgLoader, const std::vector<double>& dataWeights,
            const std::vector<int>& dataY) const
    {
        return NULL;
    }

    void AdaBoostFaceDetection::Reset(void)
    {

    }

    RealTimeFaceDetection::RealTimeFaceDetection()
    {
    }

    RealTimeFaceDetection::~RealTimeFaceDetection()
    {
    }
}

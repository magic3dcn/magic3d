#include "ExplicitShapeRegression.h"
#include "../Tool/ErrorCodes.h"
#include "../Tool/LogSystem.h"

namespace MagicDIP
{
    ExplicitShapeRegression::ExplicitShapeRegression()
    {
    }

    ExplicitShapeRegression::~ExplicitShapeRegression()
    {
    }

    int ExplicitShapeRegression::LearnRegression(const std::vector<std::string>& imgFiles, const std::vector<double>& initTheta, 
            const std::vector<double>& finalTheta, int keyPointCount, int outerCount, int innerCount, int fernSize, int featureSize)
    {
        return MAGIC_NO_ERROR;
    }
     
    int ExplicitShapeRegression::ShapeRegression(const cv::Mat& img, const std::vector<double>& initTheta, 
        std::vector<double>&finalTheta) const
    {
        return MAGIC_NO_ERROR;
    }
        
    void ExplicitShapeRegression::Save(const std::string& fileName) const
    {

    }
        
    void ExplicitShapeRegression::Load(const std::string& fileName)
    {

    }

}

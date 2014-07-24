#include "FaceDetection.h"
#include "../DIP/RealTimeFaceDetection.h"
#include "../Tool/ErrorCodes.h"
#include "../Tool/LogSystem.h"
#include <fstream>

namespace MagicApp
{
    FaceDetection::FaceDetection() :
        mDm(DM_Default),
        mpRealTimeDetector(NULL)
    {
    }

    FaceDetection::~FaceDetection()
    {
        if (mpRealTimeDetector != NULL)
        {
            delete mpRealTimeDetector;
            mpRealTimeDetector = NULL;
        }
    }

    int FaceDetection::LearnDetector(const std::string& imgFile, DetectionMethod dm)
    {
        switch (dm)
        {
        case DM_Default:
            return LearnRealTimeDetector(imgFile);
        default:
            return MAGIC_INVALID_INPUT;
        }
    }
     
    int FaceDetection::DetectFace(const cv::Mat& img, std::vector<int>& faces)
    {
        switch (mDm)
        {
        case MagicApp::FaceDetection::DM_Default:
            if (mpRealTimeDetector == NULL)
            {
                return 0;
            }
            else
            {
                return mpRealTimeDetector->Detect(img, faces);
            }
        default:
            return 0;
        }
    }
     
    void FaceDetection::Save(const std::string& fileName) const
    {
        std::ofstream fout(fileName);
        fout << mDm << std::endl;
        std::string detectorFile = fileName;
        std::string::size_type pos = detectorFile.rfind(".");
        switch (mDm)
        {
        case DM_Default:
            detectorFile.replace(pos, 4, ".abfd");
            fout << detectorFile << std::endl;
            if (mpRealTimeDetector != NULL)
            {
                mpRealTimeDetector->Save(detectorFile);
            }
            else
            {
                DebugLog << "Error: AdaBoostDetector is NULL" << std::endl;
            }
            break;
        default:
            break;
        }
        fout.close();
    }
     
    void FaceDetection::Load(const std::string& fileName)
    {
        std::ifstream fin(fileName);
        int dm;
        fin >> dm;
        mDm = DetectionMethod(dm);
        std::string detectorFile;
        fin >> detectorFile;
        fin.close();
        switch (mDm)
        {
        case DM_Default:
            if (mpRealTimeDetector == NULL)
            {
                mpRealTimeDetector = new MagicDIP::RealTimeFaceDetection;
            }
            mpRealTimeDetector->Load(detectorFile);
            break;
        default:
            break;
        }
    }

    int FaceDetection::LearnRealTimeDetector(const std::string& imgFile)
    {
        if (mpRealTimeDetector == NULL)
        {
            mpRealTimeDetector = new MagicDIP::RealTimeFaceDetection;
        }
        return MAGIC_NO_ERROR;
    }
}

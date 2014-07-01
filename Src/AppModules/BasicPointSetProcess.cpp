#include "BasicPointSetProcess.h"
#include "../Tool/LogSystem.h"
#include "../DGP/PointCloud3D.h"
#include "../DGP/Parser.h"
#include "../DGP/SignedDistanceFunction.h"
#include "../DGP/Registration.h"
#include <sstream>

namespace MagicApp
{
    BasicPointSetProcess::BasicPointSetProcess() :
        mpPointSet(NULL)
    {
    }

    BasicPointSetProcess::~BasicPointSetProcess()
    {
    }

    bool BasicPointSetProcess::LoadPointSet(const std::string& fileName)
    {
        return true;
    }

    const MagicDGP::Point3DSet* BasicPointSetProcess::GetPointSet(void) const
    {
        return NULL;
    }

    MagicDGP::Point3DSet* BasicPointSetProcess::GetPointSet(void)
    {
        return NULL;
    }

    void BasicPointSetProcess::SavePointSet(const std::string& fileName)
    {

    }

    void BasicPointSetProcess::PointSetRegistration(const std::string& path, const std::vector<int>& pointIndex, 
        const std::vector<double>& boundingBox)
    {
        DebugLog << "Create SignedDistanceFunction" << std::endl;
        MagicDGP::SignedDistanceFunction sdf(400, 400, 400, boundingBox.at(0), boundingBox.at(1), boundingBox.at(2), 
            boundingBox.at(3), boundingBox.at(4), boundingBox.at(5));
        DebugLog << "Create SignedDistanceFunction Finish" << std::endl;
        MagicMath::HomoMatrix4 lastTrans;
        lastTrans.Unit();
        std::stringstream ss;
        ss << path << pointIndex.at(0) << ".obj";
        std::string pointFileName;
        ss >> pointFileName;
        mpPointSet = MagicDGP::Parser::ParsePointSet(pointFileName);
        sdf.UpdateFineSDF(mpPointSet, &lastTrans);
        delete mpPointSet;
        mpPointSet = sdf.ExtractFinePointCloud();
        //MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("ScannerDepth", "MyCookTorrancePoint", pPointSet);
        //MagicCore::RenderSystem::GetSingleton()->Update();
        int frameCount = pointIndex.size();
        for (int frameIndex = 1; frameIndex < frameCount; frameIndex++)
        {
            DebugLog << "Fusion Point Set: " << frameIndex << " -------------------------------"<< std::endl;
            ss.clear();
            pointFileName.clear();
            ss << path << pointIndex.at(frameIndex) << ".obj";
            MagicDGP::Point3DSet* pNewPC = MagicDGP::Parser::ParsePointSet(pointFileName);
            MagicMath::HomoMatrix4 newTrans;//
            MagicDGP::Registration registrate;
            //registrate.ICPRegistrateEnhance(mpPointSet, pNewPC, &lastTrans, &newTrans, mDepthStream);//
            MagicMath::HomoMatrix4 newTransInv = newTrans.Inverse();//
            sdf.UpdateSDF(pNewPC, &newTransInv);//
            lastTrans = newTrans;
            delete mpPointSet;
            delete pNewPC;
            pNewPC = NULL;
            mpPointSet = sdf.ExtractFinePointCloud();//
        }
    }

    void BasicPointSetProcess::Clear(void)
    {
        if (mpPointSet != NULL)
        {
            delete mpPointSet;
            mpPointSet = NULL;
        }
    }
}

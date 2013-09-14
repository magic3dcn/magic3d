#include "StdAfx.h"
#include "Reconstruction.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../DGP/Filter.h"
#include "../DGP/Registration.h"
#include "../DGP/Parser.h"
#include "../Common/ToolKit.h"

namespace MagicApp
{
    Reconstruction::Reconstruction() :
        mSdf(512, 512, 512, -1000.f, 1000.f, -1000.f, 1000.f, 500.f, 2500.f)
    {

    }

    Reconstruction::~Reconstruction()
    {

    }

    bool Reconstruction::Enter(void)
    {
        MagicLog << "Enter Reconstruction" << std::endl;
        mUI.Setup();
        SetupScene();
        return true;
    }

    bool Reconstruction::Update(float timeElapsed)
    {
        return true;
    }
    
    bool Reconstruction::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();
        return true;
    }

    bool Reconstruction::MouseMoved( const OIS::MouseEvent &arg )
    {
        mViewTool.MouseMoved(arg);
        return true;
    }

    bool Reconstruction::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        mViewTool.MousePressed(arg);
        return true;
    }

    std::string Reconstruction::AddPoint3DSet(MagicDGP::Point3DSet* pPS)
    {
        int psNum = mPCSet.size();
        char pcName[16];
        sprintf(pcName, "PC%d", psNum);
        std::string pcNameStr(pcName);
        mPCSet[pcNameStr] = pPS;

        return pcNameStr;
    }

    void Reconstruction::UpdatePCRendering()
    {
        for (std::map<std::string, MagicDGP::Point3DSet* >::iterator itr = mPCSet.begin(); itr != mPCSet.end(); itr++)
        {
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet(itr->first, "SimplePoint", itr->second);
        }
    }

    void Reconstruction::FilterPointSet()
    {
        for (std::map<std::string, MagicDGP::Point3DSet* >::iterator itr = mPCSet.begin(); itr != mPCSet.end(); itr++)
        {
            MagicDGP::Filter::FilterDepthScanPointSet(itr->second);
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet(itr->first, "SimplePoint", itr->second);
        }
    }

    void Reconstruction::AlignPointSet()
    {
        if (mPCSet.size() > 1)
        {
            std::map<std::string, MagicDGP::Point3DSet* >::iterator itrRef = mPCSet.begin();
            std::map<std::string, MagicDGP::Point3DSet* >::iterator itrPC  = ++(mPCSet.begin());
            for ( ; itrPC != mPCSet.end(); itrPC++, itrRef++)
            {
                MagicDGP::Registration::ICPRegistrate(itrRef->second, itrPC->second);
            }
        }
    }

    void Reconstruction::TSDFExtraction()
    {
        MagicDGP::SignedDistanceFunction sdf(512, 512, 512, -1000.f, 1000.f, -1000.f, 1000.f, 500.f, 2500.f);
        MagicDGP::HomoMatrix4 transform;
        sdf.UpdateSDF(mPCSet["PC0"], &transform);
        MagicDGP::Point3DSet* pPC = sdf.ExtractPointCloud();
        MagicDGP::Parser::ExportPointSet("extract.obj", pPC);
        //delete pPC;
    }

    void Reconstruction::PointSetFusion()
    {
        //MagicDGP::SignedDistanceFunction sdf(512, 512, 512, -1000.f, 1000.f, -1000.f, 1000.f, 500.f, 2500.f);
        MagicDGP::HomoMatrix4 lastTrans;
        //Init lastTrans from file
        std::ifstream fin("../../Media/Model/Transform_10.txt");
        for (int rowIdx = 0; rowIdx < 4; rowIdx++)
        {
            for (int colIdx = 0; colIdx < 4; colIdx++)
            {
                float t;
                fin >> t;
                lastTrans.SetValue(rowIdx, colIdx, t);
            }
        }
        //
        char fileName[50] = "../../Media/Model/Fusion_10.obj";
        MagicDGP::Point3DSet* pRefPC = MagicDGP::Parser::ParsePointSet(fileName);
        MagicLog << "Update refPC" << std::endl;
        MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("refPC", "SimplePoint_Red", pRefPC);
        MagicCore::RenderSystem::GetSingleton()->Update();
        int fileStartIndex = 11;
        int fileEndIndex = 575;
        for (int i = fileStartIndex; i <= fileEndIndex; i++)
        {
            MagicLog << "Fusion Point Set: " << i << " -------------------------------"<< std::endl;
            sprintf(fileName, "../../Media/Model/Scene_%d.obj", i);
            MagicDGP::Point3DSet* pNewPC = MagicDGP::Parser::ParsePointSet(fileName);//
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("newPC", "SimplePoint_Green", pNewPC);
            MagicCore::RenderSystem::GetSingleton()->Update();
            MagicDGP::HomoMatrix4 newTrans;
            MagicLog << "Fusion: ICP Registration" << std::endl;
            MagicDGP::Registration::ICPRegistrate(pRefPC, pNewPC, &lastTrans, &newTrans);//
            MagicLog << "Fusion: Update SDF" << std::endl;
            mSdf.UpdateSDF(pNewPC, &newTrans);//
            lastTrans = newTrans;
            delete pRefPC;
            delete pNewPC;
            pNewPC = NULL;
            MagicLog << "Fusion: Extract Point Set" << std::endl;
            pRefPC = mSdf.ExtractPointCloud();//
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("refPC", "SimplePoint_Red", pRefPC);
            MagicCore::RenderSystem::GetSingleton()->Update();
            char exportName[50];
            sprintf(exportName, "../../Media/Model/Fusion_%d.obj", i);
            MagicDGP::Parser::ExportPointSet(exportName, pRefPC);//
            //export transform
            sprintf(exportName, "../../Media/Model/Transform_%d.txt", i);
            std::ofstream fout(exportName);
            for (int rowIdx = 0; rowIdx < 4; rowIdx++)
            {
                for (int colIdx = 0; colIdx < 4; colIdx++)
                {
                    fout << lastTrans.GetValue(rowIdx, colIdx) << " ";
                }
                fout << std::endl;
            }
            fout.close();
            //
        }
    }

    void Reconstruction::Clear()
    {

    }

    void Reconstruction::SetupScene(void)
    {
        MagicLog << "Reconstruction::SetupScene" << std::endl;
        mViewTool.SetScale(5000.f);
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        Ogre::Camera* pCamera = MagicCore::RenderSystem::GetSingleton()->GetMainCamera();
        pCamera->setPosition(0, 0, 5000);
        pCamera->lookAt(Ogre::Vector3(0,0,0));
        pCamera->setNearClipDistance(50);
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->yaw(Ogre::Degree(180.f), Ogre::Node::TS_PARENT);
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light*  sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(10000, 10000, 20000);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void Reconstruction::ShutdownScene(void)
    {
        MagicLog << "Reconstruction::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        Ogre::Camera* pCamera = MagicCore::RenderSystem::GetSingleton()->GetMainCamera();
        pCamera->setPosition(0, 0, 4);
        pCamera->lookAt(Ogre::Vector3(0,0,0));
        pCamera->setNearClipDistance(0.05);
    }
}
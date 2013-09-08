#include "StdAfx.h"
#include "Reconstruction.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../DGP/Filter.h"
#include "../DGP/Registration.h"
#include "../DGP/SignedDistanceFunction.h"
#include "../DGP/Parser.h"

namespace MagicApp
{
    Reconstruction::Reconstruction()
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
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet(itr->second, itr->first, "SimplePoint");
        }
    }

    void Reconstruction::FilterPointSet()
    {
        for (std::map<std::string, MagicDGP::Point3DSet* >::iterator itr = mPCSet.begin(); itr != mPCSet.end(); itr++)
        {
            MagicDGP::Filter::FilterDepthScanPointSet(itr->second);
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet(itr->second, itr->first, "SimplePoint");
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
        float tranform[12] = {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f};
        sdf.UpdateSDF(mPCSet["PC0"], tranform);
        MagicDGP::Point3DSet* pPC = sdf.ExtractPointCloud();
        MagicDGP::Parser::ExportPointSet("extract.obj", pPC);
        //delete pPC;
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
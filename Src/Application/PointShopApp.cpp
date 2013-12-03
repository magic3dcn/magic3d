#include "PointShopApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../DGP/Consolidation.h"

namespace MagicApp
{
    PointShopApp::PointShopApp() :
        mpPointSet(NULL)
    {
    }

    PointShopApp::~PointShopApp()
    {
        if (mpPointSet != NULL)
        {
            delete mpPointSet;
            mpPointSet = NULL;
        }
    }

    bool PointShopApp::Enter(void)
    {
        InfoLog << "Enter PointShopApp" << std::endl; 
        mUI.Setup();
        SetupScene();

        return true;
    }

    bool PointShopApp::Update(float timeElapsed)
    {
        return true;
    }

    bool PointShopApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();
        return true;
    }

    bool PointShopApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        mViewTool.MouseMoved(arg);
        return true;
    }

    bool PointShopApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        mViewTool.MousePressed(arg);
        return true;
    }

    bool PointShopApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        return true;
    }

    bool PointShopApp::OpenPointSet(bool& hasNormal)
    {
        std::string fileName;
        char filterName[] = "OBJ Files(*.obj)\0*.obj\0STL Files(*.stl)\0*.stl\0OFF Files(*.off)\0*.off\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            MagicDGP::Point3DSet* pPointSet = MagicDGP::Parser::ParsePointSet(fileName);
            if (pPointSet != NULL)
            {
                hasNormal = pPointSet->HasNormal();
                pPointSet->UnifyPosition(2.0);
                if (mpPointSet != NULL)
                {
                    delete mpPointSet;
                }
                mpPointSet = pPointSet;
                if (hasNormal)
                {
                    MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderPointSet", "MyCookTorrancePoint", mpPointSet);
                }
                else
                {
                    MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderPointSet", "SimplePoint", mpPointSet);
                }
                
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    void PointShopApp::SavePointSet()
    {
        if (mpPointSet != NULL)
        {
            std::string fileName;
            char filterName[] = "Support format(*.obj, *.off, *.ply)\0*.*\0";
            if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
            {
                MagicDGP::Parser::ExportPointSet(fileName, mpPointSet);
            }
        }
    }

    void PointShopApp::CalPointSetNormal()
    {
        MagicDGP::Consolidation::CalPointSetNormal(mpPointSet);
        MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderPointSet", "MyCookTorrancePoint", mpPointSet);
    }

    void PointShopApp::FlipPointSetNormal()
    {
        int pointNum = mpPointSet->GetPointNumber();
        for (int pid = 0; pid < pointNum; pid++)
        {
            MagicDGP::Vector3 nor = mpPointSet->GetPoint(pid)->GetNormal();
            nor *= -1;
            mpPointSet->GetPoint(pid)->SetNormal(nor);
        }
        MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("RenderPointSet", "MyCookTorrancePoint", mpPointSet);
    }

    void PointShopApp::SmoothPointSet()
    {

    }

    void PointShopApp::SamplePointSet()
    {

    }

    void PointShopApp::RemoveOutlier()
    {

    }

    void PointShopApp::Reconstruction()
    {

    }

    void PointShopApp::AddNoise()
    {

    }

    void PointShopApp::RectangleSelect()
    {

    }

    void PointShopApp::CycleSelect()
    {

    }

    void PointShopApp::IntelligentSelect()
    {

    }

    void PointShopApp::DeformPointSet()
    {

    }

    void PointShopApp::SetupScene(void)
    {
        InfoLog << "PointShopApp::SetupScene" << std::endl;
        //MagicCore::RenderSystem::GetSingleton()->GetRenderWindow()->getViewport(0)->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light* sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(0, 0, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void PointShopApp::ShutdownScene(void)
    {
        InfoLog << "PointShopApp::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("RenderPointSet");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetToInitialState();
    }
}
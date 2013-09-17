////#include "StdAfx.h"
#include "PointSetViewer.h"
#include "MyGUI.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../Common/RenderSystem.h"

namespace MagicApp
{
    PointSetViewer::PointSetViewer() :
        mpPointSet(NULL)
    {
    }

    PointSetViewer::~PointSetViewer()
    {
        if (mpPointSet != NULL)
        {
            delete mpPointSet;
            mpPointSet = NULL;
        }
    }

    bool PointSetViewer::Enter()
    {
        MagicLog << "Enter PointSetViewer" << std::endl;
        mUI.Setup();
        SetupScene();
        return true;
    }

    bool PointSetViewer::Update(float timeElapsed)
    {
        return true;
    }

    bool PointSetViewer::Exit()
    {
        ShutdownScene();
        mUI.Shutdown();
        return true;
    }

    void PointSetViewer::SetupScene()
    {
        MagicLog << "PointSetViewer::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light*  sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(10, 10, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void PointSetViewer::ShutdownScene()
    {
        MagicLog << "PointSetViewer::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
    }

    bool PointSetViewer::MouseMoved( const OIS::MouseEvent &arg )
    {
        mViewTool.MouseMoved(arg);
        return true;
    }

    bool PointSetViewer::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        mViewTool.MousePressed(arg);
        return true;
    }

    void PointSetViewer::SetPointSet(MagicDGP::Point3DSet* pPointSet)
    {
        if (mpPointSet != NULL)
        {
            delete mpPointSet;
        }
        mpPointSet = pPointSet;
    }
}
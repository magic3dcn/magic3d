#include "StdAfx.h"
#include "PointSetViewer.h"
#include "MyGUI.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../Common/RenderSystem.h"

namespace MagicApp
{
    PointSetViewer::PointSetViewer()
    {
    }

    PointSetViewer::~PointSetViewer()
    {
    }

    bool PointSetViewer::Enter()
    {
        MagicLog << "Enter PointSetViewer" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/Pointviewer", "FileSystem", "Pointviewer");
        MyGUI::LayoutManager::getInstance().loadLayout("PointViewerLayout.layout");
        SetupScene();
        return true;
    }

    bool PointSetViewer::Update(float timeElapsed)
    {
        return true;
    }

    bool PointSetViewer::Exit()
    {
        MagicCore::ResourceManager::GetSingleton()->UnloadResource("Homepage");
        return true;
    }

    bool PointSetViewer::Pause()
    {
        return true;
    }

    bool PointSetViewer::Resume()
    {
        return true;
    }

    void PointSetViewer::SetupScene()
    {
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light*  l0 = pSceneMgr->createLight("light0");
        l0->setPosition(10, 10, 20);
        l0->setDiffuseColour(0.8, 0.8, 0.8);
        l0->setSpecularColour(0.5, 0.5, 0.5);

        std::string fileName;
        MagicCore::ToolKit::GetSingleton()->FileOpenDlg(fileName);
        MagicDGP::Parser parser;
        MagicDGP::Point3DSet* pPointSet = parser.ParsePointSet(fileName);
        MagicLog << "Setup point cloud to scene node" << std::endl;
        MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet(pPointSet, "test", "SimplePoint");
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
}
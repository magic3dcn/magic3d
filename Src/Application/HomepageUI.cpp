////#include "StdAfx.h"
#include "HomepageUI.h"
#include "../Common/AppManager.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "PointSetViewer.h"
#include "Scan3D.h"
#include "Reconstruction.h"

namespace MagicApp
{
    HomepageUI::HomepageUI()
    {
    }

    HomepageUI::~HomepageUI()
    {

    }

    void HomepageUI::Setup()
    {
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/Homepage", "FileSystem", "Homepage");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("HomeLayout.layout");
        mRoot.at(0)->findWidget("But_Viewer")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &HomepageUI::EnterPointViewer);
        mRoot.at(0)->findWidget("But_Scan3D")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &HomepageUI::EnterScan3D);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &HomepageUI::EnterReconstruction);
    }

    void HomepageUI::Shutdown()
    {
        MagicLog << "HomepageUI::Shutdown" << std::endl;
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::GetSingleton()->UnloadResource("Homepage");
    }

    void HomepageUI::EnterPointViewer(MyGUI::Widget* pSender)
    {
        MagicLog << "EnterPointViewer Clicked" << std::endl;
        MagicCore::AppManager::GetSingleton()->EnterApp(new PointSetViewer, "PointSetViewer");
    }

    void HomepageUI::EnterScan3D(MyGUI::Widget* pSender)
    {
        MagicLog << "EnterScan3D Clicked" << std::endl;
        MagicCore::AppManager::GetSingleton()->EnterApp(new Scan3D, "Scan3D");
    }

    void HomepageUI::EnterReconstruction(MyGUI::Widget* pSender)
    {
        MagicLog << "EnterReconstruction Clicked" << std::endl;
        MagicCore::AppManager::GetSingleton()->EnterApp(new Reconstruction, "Reconstruction");
    }
}
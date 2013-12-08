////#include "StdAfx.h"
#include "HomepageUI.h"
#include "../Common/AppManager.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "PointShopApp.h"
#include "MeshShopApp.h"
#include "ScanningApp.h"
#include "ReconstructionApp.h"
#include "LeapMotionApp.h"
#include "PrimitiveDetectionApp.h"
#include "ReliefApp.h"

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
        MagicCore::ResourceManager::LoadResource("../../Media/Homepage", "FileSystem", "Homepage");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("HomeLayout.layout");
        mRoot.at(0)->findWidget("Title")->castType<MyGUI::ImageBox>()->setSize(406, 110);
        mRoot.at(0)->findWidget("But_PointShop")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &HomepageUI::EnterPointShopApp);
        mRoot.at(0)->findWidget("But_PointShop")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_MeshShop")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &HomepageUI::EnterMeshShopApp);
        mRoot.at(0)->findWidget("But_MeshShop")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Scan3D")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &HomepageUI::EnterScan3D);
        mRoot.at(0)->findWidget("But_Scan3D")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &HomepageUI::EnterReconstruction);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Relief")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &HomepageUI::EnterReliefApp);
        mRoot.at(0)->findWidget("But_Relief")->castType<MyGUI::Button>()->setSize(86, 86);
    }

    void HomepageUI::Shutdown()
    {
        InfoLog << "HomepageUI::Shutdown" << std::endl;
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("Homepage");
    }

    void HomepageUI::EnterPointShopApp(MyGUI::Widget* pSender)
    {
        InfoLog << "EnterPointShopApp Clicked" << std::endl;
        MagicCore::AppManager::GetSingleton()->EnterApp(new PointShopApp, "PointShopApp");
    }

    void HomepageUI::EnterMeshShopApp(MyGUI::Widget* pSender)
    {
        InfoLog << "EnterMeshShopApp Clicked" << std::endl;
        MagicCore::AppManager::GetSingleton()->EnterApp(new MeshShopApp, "MeshShopApp");
    }

    void HomepageUI::EnterScan3D(MyGUI::Widget* pSender)
    {
        InfoLog << "EnterScan3D Clicked" << std::endl;
        MagicCore::AppManager::GetSingleton()->EnterApp(new ScanningApp, "ScanningApp");
    }

    void HomepageUI::EnterReconstruction(MyGUI::Widget* pSender)
    {
        InfoLog << "EnterReconstruction Clicked" << std::endl;
        MagicCore::AppManager::GetSingleton()->EnterApp(new ReconstructionApp, "ReconstructionApp");
    }

    void HomepageUI::EnterReliefApp(MyGUI::Widget* pSender)
    {
        InfoLog << "EnterReliefApp Clicked" << std::endl;
        MagicCore::AppManager::GetSingleton()->EnterApp(new ReliefApp, "ReliefApp");
    }

}
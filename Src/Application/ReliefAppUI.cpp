#include "ReliefAppUI.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"
#include "../Common/AppManager.h"
#include "../Common/ToolKit.h"
#include "ReliefApp.h"

namespace MagicApp
{
    ReliefAppUI::ReliefAppUI()
    {
    }

    ReliefAppUI::~ReliefAppUI()
    {
    }

    void ReliefAppUI::Setup()
    {
        MagicCore::ResourceManager::LoadResource("../../Media/ReliefApp", "FileSystem", "ReliefApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("ReliefApp.layout");
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReliefAppUI::OpenPointCloud);
        mRoot.at(0)->findWidget("But_Relief")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReliefAppUI::GenerateRelief);
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReliefAppUI::ExportReliefMesh);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReliefAppUI::BackHome);
        mRoot.at(0)->findWidget("But_Contact")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReliefAppUI::Contact);
    }

    void ReliefAppUI::Shutdown()
    {
        InfoLog << "ReliefAppUI::Shutdown" << std::endl;
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("ReliefApp");
    }

    void ReliefAppUI::OpenPointCloud(MyGUI::Widget* pSender)
    {
        ReliefApp* pRelief = dynamic_cast<ReliefApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReliefApp"));
        if (pRelief != NULL)
        {
            if (pRelief->ImportPointSet())
            {
                mRoot.at(0)->findWidget("But_Relief")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setEnabled(false);
            }
        }
    }

    void ReliefAppUI::GenerateRelief(MyGUI::Widget* pSender)
    {
        ReliefApp* pRelief = dynamic_cast<ReliefApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReliefApp"));
        if (pRelief != NULL)
        {
            pRelief->GenerateRelief();
            mRoot.at(0)->findWidget("But_Relief")->castType<MyGUI::Button>()->setEnabled(false);
            mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setEnabled(true);
        }
    }

    void ReliefAppUI::ExportReliefMesh(MyGUI::Widget* pSender)
    {
        ReliefApp* pRelief = dynamic_cast<ReliefApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReliefApp"));
        if (pRelief != NULL)
        {
            pRelief->ExportReliefMesh();
        }
    }

    void ReliefAppUI::BackHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }

    void ReliefAppUI::Contact(MyGUI::Widget* pSender)
    {
        MagicCore::ToolKit::OpenWebsite(std::string("http://magic3dcn.wix.com/magic3d#!form__map/c24vq"));
    }
}
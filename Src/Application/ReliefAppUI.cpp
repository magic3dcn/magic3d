#include "ReliefAppUI.h"
#include "../Common/LogSystem.h"
#include "../Common/ResourceManager.h"
#include "../Common/AppManager.h"
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
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->setSize(86, 86);
        mRoot.at(0)->findWidget("But_Relief")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReliefAppUI::GenerateRelief);
        mRoot.at(0)->findWidget("But_Relief")->castType<MyGUI::Button>()->setSize(86, 86);
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReliefAppUI::ExportReliefMesh);
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setSize(86, 86);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReliefAppUI::BackHome);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setSize(86, 86);
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
                mRoot.at(0)->findWidget("But_Relief")->castType<MyGUI::Button>()->setVisible(true);
                mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setVisible(false);
            }
        }
    }

    void ReliefAppUI::GenerateRelief(MyGUI::Widget* pSender)
    {
        ReliefApp* pRelief = dynamic_cast<ReliefApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReliefApp"));
        if (pRelief != NULL)
        {
            pRelief->GenerateRelief();
            mRoot.at(0)->findWidget("But_Relief")->castType<MyGUI::Button>()->setVisible(false);
            mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setVisible(true);
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
}
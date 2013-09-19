#include "ScanningAppUI.h"
#include "ScanningApp.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/AppManager.h"

namespace MagicApp
{
    ScanningAppUI::ScanningAppUI() :
        mIsRecording(false)
    {
    }

    ScanningAppUI::~ScanningAppUI()
    {
    }

    void ScanningAppUI::Setup()
    {
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/Scanning", "FileSystem", "Scanning");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("ScanningAppLayout.layout");
        mRoot.at(0)->findWidget("But_Record")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ScanningAppUI::RecordDepth);
        mRoot.at(0)->findWidget("But_Record")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ScanningAppUI::BackHome);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setSize(86, 87);
    }

    void ScanningAppUI::Shutdown()
    {
        MagicLog << "ScanningAppUI::Shutdown" << std::endl;
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::GetSingleton()->UnloadResource("Scanning");
    }

    void ScanningAppUI::RecordDepth(MyGUI::Widget* pSender)
    {
        if (!mIsRecording)
        {
            mRoot.at(0)->findWidget("But_Record")->castType<MyGUI::Button>()->changeWidgetSkin("But_Save");
            mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setVisible(false);
            mIsRecording = true;
            ScanningApp* pSA = dynamic_cast<ScanningApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ScanningApp"));
            pSA->StartRecord();
        }
        else
        {
            mRoot.at(0)->findWidget("But_Record")->castType<MyGUI::Button>()->changeWidgetSkin("But_RecordDepth");
            mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setVisible(true);
            mIsRecording = false;
            ScanningApp* pSA = dynamic_cast<ScanningApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ScanningApp"));
            pSA->StopRecord();
        }
    }

    void ScanningAppUI::BackHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}
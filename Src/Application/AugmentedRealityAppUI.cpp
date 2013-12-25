#include "AugmentedRealityAppUI.h"
#include "AugmentedRealityApp.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../Common/AppManager.h"

namespace MagicApp
{
    AugmentedRealityAppUI::AugmentedRealityAppUI()
    {
    }

    AugmentedRealityAppUI::~AugmentedRealityAppUI()
    {
    }

    void AugmentedRealityAppUI::Setup()
    {
        InfoLog << "AugmentedRealityAppUI::Setup" << std::endl;
        MagicCore::ResourceManager::LoadResource("../../Media/AugmentedRealityApp", "FileSystem", "AugmentedRealityApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("AugmentedRealityApp.layout");
        mRoot.at(0)->findWidget("But_OpenVideo")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &AugmentedRealityAppUI::OpenVideo);
        mRoot.at(0)->findWidget("But_CameraCapture")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &AugmentedRealityAppUI::CameraCapture);
        mRoot.at(0)->findWidget("But_CameraRecord")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &AugmentedRealityAppUI::CameraRecord);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &AugmentedRealityAppUI::BackToHome);
    }

    void AugmentedRealityAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("AugmentedRealityApp");
    }

    void AugmentedRealityAppUI::OpenVideo(MyGUI::Widget* pSender)
    {
        AugmentedRealityApp* pARA = dynamic_cast<AugmentedRealityApp* >(MagicCore::AppManager::GetSingleton()->GetApp("AugmentedRealityApp"));
        if (pARA != NULL)
        {
            pARA->OpenVideo();
        }
    }

    void AugmentedRealityAppUI::CameraCapture(MyGUI::Widget* pSender)
    {

    }

    void AugmentedRealityAppUI::CameraRecord(MyGUI::Widget* pSender)
    {

    }

    void AugmentedRealityAppUI::BackToHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}
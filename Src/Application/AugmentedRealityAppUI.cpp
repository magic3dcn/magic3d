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
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &AugmentedRealityAppUI::BackToHome);
    }

    void AugmentedRealityAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("AugmentedRealityApp");
    }

    void AugmentedRealityAppUI::BackToHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}
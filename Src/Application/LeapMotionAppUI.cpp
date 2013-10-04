#include "LeapMotionAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../Common/AppManager.h"

namespace MagicApp
{
    LeapMotionAppUI::LeapMotionAppUI()
    {
    }

    LeapMotionAppUI::~LeapMotionAppUI()
    {
    }

    void LeapMotionAppUI::Setup()
    {
        MagicLog << "LeapMotionAppUI::Setup" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/LeapMotionApp", "FileSystem", "LeapMotionApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("LeapMotionAppUI.layout");
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &LeapMotionAppUI::OpenMesh3D);
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &LeapMotionAppUI::BackToHome);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->setSize(86, 87);
    }

    void LeapMotionAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::GetSingleton()->UnloadResource("LeapMotionApp");
    }

    void LeapMotionAppUI::OpenMesh3D(MyGUI::Widget* pSender)
    {

    }

    void LeapMotionAppUI::BackToHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}
#include "StdAfx.h"
#include "ReconstructionUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/AppManager.h"

namespace MagicApp
{
    ReconstructionUI::ReconstructionUI()
    {

    }

    ReconstructionUI::~ReconstructionUI()
    {

    }

    void ReconstructionUI::Setup()
    {
        MagicLog << "ReconstructionUI::Setup" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/Reconstruction", "FileSystem", "Reconstruction");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("ReconstructionLayout.layout");
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionUI::OpenPointSet);
        mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionUI::FilterPointSet);
        mRoot.at(0)->findWidget("But_Align")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionUI::AlignPointSet);
        mRoot.at(0)->findWidget("But_BackToHomepage")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionUI::BackToHomepage);
    }

    void ReconstructionUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::GetSingleton()->UnloadResource("Reconstruction");
    }

    void ReconstructionUI::OpenPointSet(MyGUI::Widget* pSender)
    {
        MagicLog << "ReconstructionUI::OpenPointSet" << std::endl;
    }

    void ReconstructionUI::FilterPointSet(MyGUI::Widget* pSender)
    {
        MagicLog << "ReconstructionUI::FilterPointSet" << std::endl;
    }

    void ReconstructionUI::AlignPointSet(MyGUI::Widget* pSender)
    {
        MagicLog << "ReconstructionUI::AlignPointSet" << std::endl;
    }

    void ReconstructionUI::BackToHomepage(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}
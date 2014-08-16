#include "stdafx.h"
#include "AugmentedRealityAppUI.h"
#include "AugmentedRealityApp.h"
#include "../Common/ResourceManager.h"
#include "Tool/LogSystem.h"
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
        mRoot.at(0)->findWidget("Slider_VideoFrame")->castType<MyGUI::ScrollBar>()->eventScrollChangePosition += MyGUI::newDelegate(this, &AugmentedRealityAppUI::ChangeVideoFrame);
        mRoot.at(0)->findWidget("But_CaptureFrame")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &AugmentedRealityAppUI::CaptureFrame);
        mRoot.at(0)->findWidget("But_ClearCapture")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &AugmentedRealityAppUI::ClearCapture);
        mRoot.at(0)->findWidget("But_NextImage")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &AugmentedRealityAppUI::NextImage);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &AugmentedRealityAppUI::BackToHome);
    }

    void AugmentedRealityAppUI::SetupImageBrowsing()
    {
        mRoot.at(0)->findWidget("Slider_VideoFrame")->castType<MyGUI::ScrollBar>()->setEnabled(false);
        mRoot.at(0)->findWidget("But_CaptureFrame")->castType<MyGUI::Button>()->setEnabled(false);
        mRoot.at(0)->findWidget("But_ClearCapture")->castType<MyGUI::Button>()->setEnabled(false);
        mRoot.at(0)->findWidget("But_NextImage")->castType<MyGUI::Button>()->setEnabled(true);
    }

    void AugmentedRealityAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("AugmentedRealityApp");
    }

    void AugmentedRealityAppUI::SetSliderPosition(int pos)
    {
        mRoot.at(0)->findWidget("Slider_VideoFrame")->castType<MyGUI::ScrollBar>()->setScrollPosition(pos);
    }

    void AugmentedRealityAppUI::SetSliderRange(int range)
    {
        mRoot.at(0)->findWidget("Slider_VideoFrame")->castType<MyGUI::ScrollBar>()->setScrollRange(range);
    }

    void AugmentedRealityAppUI::OpenVideo(MyGUI::Widget* pSender)
    {
        AugmentedRealityApp* pARA = dynamic_cast<AugmentedRealityApp* >(MagicCore::AppManager::GetSingleton()->GetApp("AugmentedRealityApp"));
        if (pARA != NULL)
        {
            if (pARA->OpenVideo() == true)
            {
                mRoot.at(0)->findWidget("Slider_VideoFrame")->castType<MyGUI::ScrollBar>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_CaptureFrame")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_ClearCapture")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_NextImage")->castType<MyGUI::Button>()->setEnabled(false);
            }
        }
    }

    void AugmentedRealityAppUI::ChangeVideoFrame(MyGUI::ScrollBar* pSender, size_t pos)
    {
        AugmentedRealityApp* pARA = dynamic_cast<AugmentedRealityApp* >(MagicCore::AppManager::GetSingleton()->GetApp("AugmentedRealityApp"));
        if (pARA != NULL)
        {
            pARA->SetFramePosition(pos);
        }
    }

    void AugmentedRealityAppUI::CaptureFrame(MyGUI::Widget* pSender)
    {
        AugmentedRealityApp* pARA = dynamic_cast<AugmentedRealityApp* >(MagicCore::AppManager::GetSingleton()->GetApp("AugmentedRealityApp"));
        if (pARA != NULL)
        {
            pARA->CaptureFrame();
        }
    }

    void AugmentedRealityAppUI::ClearCapture(MyGUI::Widget* pSender)
    {
        AugmentedRealityApp* pARA = dynamic_cast<AugmentedRealityApp* >(MagicCore::AppManager::GetSingleton()->GetApp("AugmentedRealityApp"));
        if (pARA != NULL)
        {
            pARA->ClearCapture();
        }
    }

    void AugmentedRealityAppUI::NextImage(MyGUI::Widget* pSender)
    {
        AugmentedRealityApp* pARA = dynamic_cast<AugmentedRealityApp* >(MagicCore::AppManager::GetSingleton()->GetApp("AugmentedRealityApp"));
        if (pARA != NULL)
        {
            pARA->NextImage();
        }
    }

    void AugmentedRealityAppUI::BackToHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}
#include "ReconstructionAppUI.h"
#include "../Common/ResourceManager.h"
#include "Tool/LogSystem.h"
#include "../Common/AppManager.h"
#include "../Common/ToolKit.h"
#include "ReconstructionApp.h"
#include "../Common/RenderSystem.h"

namespace MagicApp
{
    ReconstructionAppUI::ReconstructionAppUI() :
        mFrameStartIndex(0),
        mFrameEndIndex(0)
    {
    }

    ReconstructionAppUI::~ReconstructionAppUI()
    {
    }

    void ReconstructionAppUI::Setup()
    {
        InfoLog << "ReconstructionUI::Setup" << std::endl;
        SetupRecordPlayback();
    }

    void ReconstructionAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("ReconstructionApp");
    }

    void ReconstructionAppUI::SetupRecordPlayback()
    {
        InfoLog << "ReconstructionUI::SetupRecordPlayback" << std::endl;
        MagicCore::ResourceManager::LoadResource("../../Media/ReconstructionApp", "FileSystem", "ReconstructionApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("RecordPlayback.layout");
        mRoot.at(0)->findWidget("But_OpenRecord")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::OpenScanRecord);
        mRoot.at(0)->findWidget("But_OpenRecord")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::BackHome);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setSize(40, 40);
        mRoot.at(0)->findWidget("But_Contact")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::Contact);
        mRoot.at(0)->findWidget("But_Contact")->castType<MyGUI::Button>()->setSize(40, 40);
        mRoot.at(0)->findWidget("But_Align")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::PointSetAlign);
        mRoot.at(0)->findWidget("But_Align")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_ExportPointSet")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::ExportPointSet);
        mRoot.at(0)->findWidget("But_ExportPointSet")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_Down")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::ChangeDownRange);
        mRoot.at(0)->findWidget("But_Down")->castType<MyGUI::Button>()->setSize(50, 50);
        mRoot.at(0)->findWidget("But_Top")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::ChangeTopRange);
        mRoot.at(0)->findWidget("But_Top")->castType<MyGUI::Button>()->setSize(50, 50);
        mRoot.at(0)->findWidget("But_Left")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::ChangeLeftRange);
        mRoot.at(0)->findWidget("But_Left")->castType<MyGUI::Button>()->setSize(50, 50);
        mRoot.at(0)->findWidget("But_Right")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::ChangeRightRange);
        mRoot.at(0)->findWidget("But_Right")->castType<MyGUI::Button>()->setSize(50, 50);
        mRoot.at(0)->findWidget("But_Front")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::ChangeFrontRange);
        mRoot.at(0)->findWidget("But_Front")->castType<MyGUI::Button>()->setSize(50, 50);
        mRoot.at(0)->findWidget("But_Back")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::ChangeBackRange);
        mRoot.at(0)->findWidget("But_Back")->castType<MyGUI::Button>()->setSize(50, 50);
        mRoot.at(0)->findWidget("Slider_FrameStart")->castType<MyGUI::ScrollBar>()->eventScrollChangePosition += MyGUI::newDelegate(this, &ReconstructionAppUI::ChangeFrameStart);
        mRoot.at(0)->findWidget("Slider_FrameEnd")->castType<MyGUI::ScrollBar>()->eventScrollChangePosition += MyGUI::newDelegate(this, &ReconstructionAppUI::ChangeFrameEnd);
    }

    void ReconstructionAppUI::SetupReconstructProgress()
    {
        InfoLog << "ReconstructionUI::SetupReconstructProgress" << std::endl;
        mRoot.at(0)->findWidget("Progress_Registrate")->castType<MyGUI::ProgressBar>()->setVisible(true);
        mRoot.at(0)->findWidget("But_Align")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_ExportPointSet")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Down")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Top")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Left")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Right")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Front")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Back")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_OpenRecord")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("Slider_FrameStart")->castType<MyGUI::ScrollBar>()->setVisible(false);
        mRoot.at(0)->findWidget("Slider_FrameEnd")->castType<MyGUI::ScrollBar>()->setVisible(false);
    }

    void ReconstructionAppUI::SetProgressBarPosition(int pos)
    {
        mRoot.at(0)->findWidget("Progress_Registrate")->castType<MyGUI::ProgressBar>()->setProgressPosition(pos);
    }

    void ReconstructionAppUI::SetProgressBarRange(int range)
    {
        mRoot.at(0)->findWidget("Progress_Registrate")->castType<MyGUI::ProgressBar>()->setProgressRange(range);
    }

    void ReconstructionAppUI::OpenScanRecord(MyGUI::Widget* pSender)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        int frameNum;
        if (pRA->OpenSceneRecord(frameNum))
        {
            mRoot.at(0)->findWidget("But_Align")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_ExportPointSet")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Down")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Top")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Left")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Right")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Front")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Back")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("Slider_FrameStart")->castType<MyGUI::ScrollBar>()->setVisible(true);
            mRoot.at(0)->findWidget("Slider_FrameStart")->castType<MyGUI::ScrollBar>()->setScrollRange(frameNum);
            mRoot.at(0)->findWidget("Slider_FrameStart")->castType<MyGUI::ScrollBar>()->setScrollPosition(0);
            mRoot.at(0)->findWidget("Slider_FrameEnd")->castType<MyGUI::ScrollBar>()->setVisible(true);
            mRoot.at(0)->findWidget("Slider_FrameEnd")->castType<MyGUI::ScrollBar>()->setScrollRange(frameNum);
            mRoot.at(0)->findWidget("Slider_FrameEnd")->castType<MyGUI::ScrollBar>()->setScrollPosition(frameNum - 1);
            mFrameStartIndex = 0;
            mFrameEndIndex = frameNum - 1;
        }
    }

    void ReconstructionAppUI::BackHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }

    void ReconstructionAppUI::Contact(MyGUI::Widget* pSender)
    {
        MagicCore::ToolKit::OpenWebsite(std::string("http://magic3dcn.wix.com/magic3d#!form__map/c24vq"));
    }

    void ReconstructionAppUI::ChangeFrameStart(MyGUI::ScrollBar* pSender, size_t pos)
    {
        if (pos > mFrameEndIndex)
        {
            pos = mFrameEndIndex;
            mRoot.at(0)->findWidget("Slider_FrameStart")->castType<MyGUI::ScrollBar>()->setScrollPosition(pos);
        }
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->SetTimeStart(pos);
        mFrameStartIndex = pos;
    }

    void ReconstructionAppUI::ChangeFrameEnd(MyGUI::ScrollBar* pSender, size_t pos)
    {
        if (pos < mFrameStartIndex)
        {
            pos = mFrameStartIndex;
            mRoot.at(0)->findWidget("Slider_FrameEnd")->castType<MyGUI::ScrollBar>()->setScrollPosition(pos);
        }
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->SetTimeEnd(pos);
        mFrameEndIndex = pos;
    }

    void ReconstructionAppUI::PointSetAlign(MyGUI::Widget* pSender)
    {
        DebugLog << "ReconstructionAppUI::PointSetAlign" << std::endl;
        SetupReconstructProgress();
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->PointSetRegistration();
    }

    void ReconstructionAppUI::ExportPointSet(MyGUI::Widget* pSender)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->ExportPointSet();
    }

    void ReconstructionAppUI::ChangeLeftRange(MyGUI::Widget* pSender, int rel)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->ChangeLeftRange(rel);
    }

    void ReconstructionAppUI::ChangeRightRange(MyGUI::Widget* pSender, int rel)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->ChangeRightRange(rel);
    }

    void ReconstructionAppUI::ChangeTopRange(MyGUI::Widget* pSender, int rel)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->ChangeTopRange(rel);
    }

    void ReconstructionAppUI::ChangeDownRange(MyGUI::Widget* pSender, int rel)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->ChangeDownRange(rel);
    }

    void ReconstructionAppUI::ChangeFrontRange(MyGUI::Widget* pSender, int rel)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->ChangeFrontRange(rel);
    }

    void ReconstructionAppUI::ChangeBackRange(MyGUI::Widget* pSender, int rel)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->ChangeBackRange(rel);
    }

}

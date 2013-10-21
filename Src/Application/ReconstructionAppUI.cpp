#include "ReconstructionAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/AppManager.h"
#include "../Common/ToolKit.h"
#include "ReconstructionApp.h"
#include "../Common/RenderSystem.h"

namespace MagicApp
{
    ReconstructionAppUI::ReconstructionAppUI() :
        mIsTimeRangeStart(true)
    {
    }

    ReconstructionAppUI::~ReconstructionAppUI()
    {
    }

    void ReconstructionAppUI::Setup()
    {
        MagicLog << "ReconstructionUI::Setup" << std::endl;
        SetupRecordPlayback();
        //SetupReconstructProgress();
        //SetupReconstructing();
        //SetupMeshProcessing();
    }

    void ReconstructionAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::GetSingleton()->UnloadResource("ReconstructionApp");
    }

    void ReconstructionAppUI::SetupRecordPlayback()
    {
        MagicLog << "ReconstructionUI::SetupRecordPlayback" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/ReconstructionApp", "FileSystem", "ReconstructionApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("RecordPlayback.layout");
        mRoot.at(0)->findWidget("But_OpenRecord")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::OpenScanRecord);
        mRoot.at(0)->findWidget("But_OpenRecord")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::BackHome);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_SetRange")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::SetTimeRange);
        mRoot.at(0)->findWidget("But_SetRange")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Align")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::PointSetAlign);
        mRoot.at(0)->findWidget("But_Align")->castType<MyGUI::Button>()->setSize(86, 87);
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
    }

    void ReconstructionAppUI::SetupReconstructProgress()
    {
        MagicLog << "ReconstructionUI::SetupReconstructProgress" << std::endl;
        //MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/ReconstructionApp", "FileSystem", "ReconstructionApp");
        //mRoot = MyGUI::LayoutManager::getInstance().loadLayout("ReconstructProgress.layout");
        mRoot.at(0)->findWidget("But_SetRange")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Align")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Down")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Top")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Left")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Right")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Front")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Back")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("But_OpenRecord")->castType<MyGUI::Button>()->setVisible(false);
        mRoot.at(0)->findWidget("Progress_Registrate")->castType<MyGUI::ProgressBar>()->setVisible(true);
    }

    void ReconstructionAppUI::SetupReconstructing()
    {
        MagicLog << "ReconstructionAppUI::SetupReconstructing" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/ReconstructionApp", "FileSystem", "ReconstructionApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("ReconstructAndMeshProcess.layout");
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::SavePointSet);
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::SmoothPointSet);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::Reconstruction);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::BackHome);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->setSize(86, 87);
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->SetupPointSetProcessing();
    }

    void ReconstructionAppUI::SetupMeshProcessing()
    {
        MagicLog << "ReconstructionAppUI::SetupMeshProcessing" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/ReconstructionApp", "FileSystem", "ReconstructionApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("ReconstructAndMeshProcess.layout");
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::SaveMesh3D);
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::SmoothMesh3D);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::BackHome);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->setVisible(false);
    }

    void ReconstructionAppUI::SetProgressBarPosition(int pos)
    {
        mRoot.at(0)->findWidget("Progress_Registrate")->castType<MyGUI::ProgressBar>()->setProgressPosition(pos);
    }

    void ReconstructionAppUI::SetProgressBarRange(int range)
    {
        mRoot.at(0)->findWidget("Progress_Registrate")->castType<MyGUI::ProgressBar>()->setProgressRange(range);
    }

    void ReconstructionAppUI::StartPostProcess()
    {
        Shutdown();
        SetupReconstructing();
    }

    void ReconstructionAppUI::OpenScanRecord(MyGUI::Widget* pSender)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        if (pRA->OpenSceneRecord())
        {
            mRoot.at(0)->findWidget("But_SetRange")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Align")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Down")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Top")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Left")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Right")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Front")->castType<MyGUI::Button>()->setVisible(true);
            mRoot.at(0)->findWidget("But_Back")->castType<MyGUI::Button>()->setVisible(true);
        }
    }

    void ReconstructionAppUI::BackHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }

    void ReconstructionAppUI::SetTimeRange(MyGUI::Widget* pSender)
    {
        if (mIsTimeRangeStart)
        {
            ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
            pRA->SetTimeStart();
            mIsTimeRangeStart = false;
            mRoot.at(0)->findWidget("But_SetRange")->castType<MyGUI::Button>()->changeWidgetSkin("But_Stop");
        }
        else
        {
            ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
            pRA->SetTimeEnd();
            mIsTimeRangeStart = true;
            mRoot.at(0)->findWidget("But_SetRange")->castType<MyGUI::Button>()->changeWidgetSkin("But_Start");
        }
    }

    void ReconstructionAppUI::PointSetAlign(MyGUI::Widget* pSender)
    {
        MagicLog << "ReconstructionAppUI::PointSetAlign" << std::endl;
        SetupReconstructProgress();
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        //pRA->PointSetRegistration();
        //pRA->PointSetRegistrationEnhance();
        pRA->PointSetRegistrationEnhance2();
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

    void ReconstructionAppUI::SavePointSet(MyGUI::Widget* pSender)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        if (pRA->SavePointSet())
        {
            mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->setVisible(true);
        }
    }

    void ReconstructionAppUI::SmoothPointSet(MyGUI::Widget* pSender)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->SmoothPointSet();
    }

    void ReconstructionAppUI::Reconstruction(MyGUI::Widget* pSender)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        if (pRA->ReconstructPointSet())
        {
            Shutdown();
            SetupMeshProcessing();
        }
    }

    void ReconstructionAppUI::SaveMesh3D(MyGUI::Widget* pSender)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        if (pRA->SaveMesh3D())
        {
            mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->setVisible(true);
        }
    }

    void ReconstructionAppUI::SmoothMesh3D(MyGUI::Widget* pSender)
    {
        ReconstructionApp* pRA = dynamic_cast<ReconstructionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("ReconstructionApp"));
        pRA->SmoothMesh3D();
    }
}
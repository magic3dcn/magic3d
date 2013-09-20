#include "ReconstructionAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/AppManager.h"
#include "../Common/ToolKit.h"

namespace MagicApp
{
    ReconstructionAppUI::ReconstructionAppUI()
    {
    }

    ReconstructionAppUI::~ReconstructionAppUI()
    {
    }

    void ReconstructionAppUI::Setup()
    {
        MagicLog << "ReconstructionUI::Setup" << std::endl;
        //MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/ReconstructionApp", "FileSystem", "ReconstructionApp");
        //mRoot = MyGUI::LayoutManager::getInstance().loadLayout("RecordPlayback.layout");
        //SetupRecordPlayback();
        //SetupReconstructProgress();
        //SetupReconstructing();
        SetupMeshProcessing();
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
        mRoot.at(0)->findWidget("But_Down")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::SetDownRange);
        mRoot.at(0)->findWidget("But_Down")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Top")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::SetTopRange);
        mRoot.at(0)->findWidget("But_Top")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Left")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::SetLeftRange);
        mRoot.at(0)->findWidget("But_Left")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Right")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::SetRightRange);
        mRoot.at(0)->findWidget("But_Right")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Front")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::SetFrontRange);
        mRoot.at(0)->findWidget("But_Front")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Back")->castType<MyGUI::Button>()->eventMouseWheel += MyGUI::newDelegate(this, &ReconstructionAppUI::SetBackRange);
        mRoot.at(0)->findWidget("But_Back")->castType<MyGUI::Button>()->setSize(86, 87);
    }

    void ReconstructionAppUI::SetupReconstructProgress()
    {
        MagicLog << "ReconstructionUI::SetupReconstructProgress" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/ReconstructionApp", "FileSystem", "ReconstructionApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("ReconstructProgress.layout");
    }

    void ReconstructionAppUI::SetupReconstructing()
    {
        MagicLog << "ReconstructionAppUI::SetupReconstructing" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/ReconstructionApp", "FileSystem", "ReconstructionApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("ReconstructAndMeshProcess.layout");
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::SavePointSet);
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_ReconAndSmooth")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::Reconstruction);
        mRoot.at(0)->findWidget("But_ReconAndSmooth")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::BackHome);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->setSize(86, 87);
    }

    void ReconstructionAppUI::SetupMeshProcessing()
    {
        MagicLog << "ReconstructionAppUI::SetupMeshProcessing" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/ReconstructionApp", "FileSystem", "ReconstructionApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("ReconstructAndMeshProcess.layout");
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::SaveMesh3D);
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_ReconAndSmooth")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::SmoothMesh3D);
        mRoot.at(0)->findWidget("But_ReconAndSmooth")->castType<MyGUI::Button>()->changeWidgetSkin("But_Smooth");
        mRoot.at(0)->findWidget("But_ReconAndSmooth")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionAppUI::BackHome);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->setSize(86, 87);
    }

    void ReconstructionAppUI::SetProgressBarPosition(int pos)
    {
        mRoot.at(0)->findWidget("Progress_Resgistrate")->castType<MyGUI::ProgressBar>()->setProgressPosition(pos);
    }

    void ReconstructionAppUI::SetProgressBarRange(int range)
    {
        mRoot.at(0)->findWidget("Progress_Resgistrate")->castType<MyGUI::ProgressBar>()->setProgressRange(range);
    }

    void ReconstructionAppUI::OpenScanRecord(MyGUI::Widget* pSender)
    {

    }

    void ReconstructionAppUI::BackHome(MyGUI::Widget* pSender)
    {

    }

    void ReconstructionAppUI::SetTimeRange(MyGUI::Widget* pSender)
    {

    }

    void ReconstructionAppUI::SetLeftRange(MyGUI::Widget* pSender, int rel)
    {

    }

    void ReconstructionAppUI::SetRightRange(MyGUI::Widget* pSender, int rel)
    {

    }

    void ReconstructionAppUI::SetTopRange(MyGUI::Widget* pSender, int rel)
    {

    }

    void ReconstructionAppUI::SetDownRange(MyGUI::Widget* pSender, int rel)
    {

    }

    void ReconstructionAppUI::SetFrontRange(MyGUI::Widget* pSender, int rel)
    {

    }

    void ReconstructionAppUI::SetBackRange(MyGUI::Widget* pSender, int rel)
    {

    }

    void ReconstructionAppUI::SavePointSet(MyGUI::Widget* pSender)
    {

    }

    void ReconstructionAppUI::Reconstruction(MyGUI::Widget* pSender)
    {

    }

    void ReconstructionAppUI::SaveMesh3D(MyGUI::Widget* pSender)
    {

    }

    void ReconstructionAppUI::SmoothMesh3D(MyGUI::Widget* pSender)
    {

    }
}
#include "stdafx.h"
#include "VideoRecordingAppUI.h"
#include "VideoRecordingApp.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/AppManager.h"

namespace MagicApp
{
    VideoRecordingAppUI::VideoRecordingAppUI() :
        mIsRecording(false)
    {
    }

    VideoRecordingAppUI::~VideoRecordingAppUI()
    {
    }

    void VideoRecordingAppUI::Setup()
    {
        MagicCore::ResourceManager::LoadResource("../../Media/VideoRecordingApp", "FileSystem", "VideoRecordingApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("VideoRecordingApp.layout");
        mRoot.at(0)->findWidget("But_Record")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VideoRecordingAppUI::Record);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &VideoRecordingAppUI::BackToHome);
    }

    void VideoRecordingAppUI::Shutdown()
    {
        InfoLog << "VideoRecordingAppUI::Shutdown" << std::endl;
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("VideoRecordingApp");
    }

    void VideoRecordingAppUI::Record(MyGUI::Widget* pSender)
    {
        if (!mIsRecording)
        {
            mRoot.at(0)->findWidget("But_Record")->castType<MyGUI::Button>()->changeWidgetSkin("But_Save");
            mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setVisible(false);
            mIsRecording = true;
            VideoRecordingApp* pVRA = dynamic_cast<VideoRecordingApp* >(MagicCore::AppManager::GetSingleton()->GetApp("VideoRecordingApp"));
            pVRA->StartRecord();
        }
        else
        {
            mRoot.at(0)->findWidget("But_Record")->castType<MyGUI::Button>()->changeWidgetSkin("But_RecordDepth");
            mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setVisible(true);
            mIsRecording = false;
            VideoRecordingApp* pVRA = dynamic_cast<VideoRecordingApp* >(MagicCore::AppManager::GetSingleton()->GetApp("VideoRecordingApp"));
            pVRA->StopRecord();
        }
    }

    void VideoRecordingAppUI::BackToHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}
////#include "StdAfx.h"
#include "Scan3DUI.h"
#include "Scan3D.h"
#include "../Common/ResourceManager.h"
#include "../Common/AppManager.h"
#include "../Common/LogSystem.h"

namespace MagicApp
{
    Scan3DUI::Scan3DUI()
    {
    }

    Scan3DUI::~Scan3DUI()
    {
    }

    void Scan3DUI::Setup()
    {
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/Scan3D", "FileSystem", "Scan3D");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("Scan3D.layout");
        mRoot.at(0)->findWidget("But_PlayScanner")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &Scan3DUI::PlayScanner);
        mRoot.at(0)->findWidget("But_Record")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &Scan3DUI::Record);
        mRoot.at(0)->findWidget("But_PlayRecordScanner")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &Scan3DUI::PlayRecordScanner);
        mRoot.at(0)->findWidget("But_Capture")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &Scan3DUI::CapturePointSet);
        mRoot.at(0)->findWidget("But_Back")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &Scan3DUI::BackToHomepage);
    }

    void Scan3DUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::GetSingleton()->UnloadResource("Scan3D");
    }

    void Scan3DUI::Record(MyGUI::Widget* pSender)
    {
        Scan3D* pScan = dynamic_cast<Scan3D* >(MagicCore::AppManager::GetSingleton()->GetApp("Scan3D"));
        pScan->Record();
        /*if (pScan->GetMediaStream().IsRecording())
        {
            pScan->GetMediaStream().StopRecorder();
        }
        else
        {
            pScan->GetMediaStream().StartRecorder();
        }*/
    }

    void Scan3DUI::PlayScanner(MyGUI::Widget* pSender)
    {
        Scan3D* pScan = dynamic_cast<Scan3D* >(MagicCore::AppManager::GetSingleton()->GetApp("Scan3D"));
        pScan->PlayScanner();
        //if (pScan->IsScannerUpdate())
        //{
        //    MagicLog << "Scan3DUI::PlayScanner StopScanner" << std::endl;
        //    pScan->GetMediaStream().StopScanner();
        //    pScan->StopUpdateScanner();
        //}
        //else
        //{
        //    MagicLog << "Scan3DUI::PlayScanner StartScanner" << std::endl;
        //    pScan->GetMediaStream().StartScanner();
        //    pScan->StartUpdateScanner();
        //}
    }

    void Scan3DUI::PlayRecordScanner(MyGUI::Widget* pSender)
    {
        Scan3D* pScan = dynamic_cast<Scan3D* >(MagicCore::AppManager::GetSingleton()->GetApp("Scan3D"));
        pScan->PlayRecordScanner();
        //if (pScan->IsScannerUpdate())
        //{
        //    MagicLog << "Scan3DUI::PlayRecordScanner StopScanner" << std::endl;
        //    pScan->GetMediaStream().StopRecordScanner();
        //    pScan->StopUpdateScanner();
        //}
        //else
        //{
        //    MagicLog << "Scan3DUI::PlayRecordScanner StartScanner" << std::endl;
        //    pScan->GetMediaStream().StartRecordScanner();
        //    pScan->StartUpdateScanner();
        //}
    }

    void Scan3DUI::CapturePointSet(MyGUI::Widget* pSender)
    {
        Scan3D* pScan = dynamic_cast<Scan3D* >(MagicCore::AppManager::GetSingleton()->GetApp("Scan3D"));
        pScan->CapturePointSet();
    }

    void Scan3DUI::BackToHomepage(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}
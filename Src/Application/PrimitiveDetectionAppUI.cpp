#include "PrimitiveDetectionAppUI.h"
#include "PrimitiveDetectionApp.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../Common/AppManager.h"
#include "../Common/RenderSystem.h"

namespace MagicApp
{
    PrimitiveDetectionAppUI::PrimitiveDetectionAppUI()
    {
    }

    PrimitiveDetectionAppUI::~PrimitiveDetectionAppUI()
    {
    }

    void PrimitiveDetectionAppUI::Setup()
    {
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "PrimitiveDetectionAppUI::Setup" << std::endl;
        MagicCore::ResourceManager::LoadResource("../../Media/PrimitiveDetection", "FileSystem", "PrimitiveDetection");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("PrimitiveDetectionLayout.layout");
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PrimitiveDetectionAppUI::OpenMesh3D);
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Ransac")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PrimitiveDetectionAppUI::RansacMethod);
        mRoot.at(0)->findWidget("But_Ransac")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PrimitiveDetectionAppUI::BackToHome);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setSize(86, 87);
    }

    void PrimitiveDetectionAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("PrimitiveDetection");
    }

    void PrimitiveDetectionAppUI::OpenMesh3D(MyGUI::Widget* pSender)
    {
        PrimitiveDetectionApp* pPD = dynamic_cast<PrimitiveDetectionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PrimitiveDetectionApp"));
        if (pPD != NULL)
        {
            pPD->ImportMesh3D();
        }
    }

    void PrimitiveDetectionAppUI::RansacMethod(MyGUI::Widget* pSender)
    {
        PrimitiveDetectionApp* pPD = dynamic_cast<PrimitiveDetectionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PrimitiveDetectionApp"));
        pPD->RansacPrimitiveDetection();
        //pPD->PrimitiveSelection(0);
    }

    void PrimitiveDetectionAppUI::BackToHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}
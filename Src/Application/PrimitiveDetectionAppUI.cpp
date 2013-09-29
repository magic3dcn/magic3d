#include "PrimitiveDetectionAppUI.h"
#include "PrimitiveDetectionApp.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
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
        MagicLog << "PrimitiveDetectionAppUI::Setup" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/PrimitiveDetection", "FileSystem", "PrimitiveDetection");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("PrimitiveDetectionLayout.layout");
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PrimitiveDetectionAppUI::OpenMesh3D);
        mRoot.at(0)->findWidget("But_Ransac")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PrimitiveDetectionAppUI::RansacMethod);
    }

    void PrimitiveDetectionAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::GetSingleton()->UnloadResource("Pointviewer");
    }

    void PrimitiveDetectionAppUI::OpenMesh3D(MyGUI::Widget* pSender)
    {
        std::string fileName;
        if (MagicCore::ToolKit::GetSingleton()->FileOpenDlg(fileName))
        {
            MagicDGP::Mesh3D* pMesh3D = MagicDGP::Parser::ParseMesh3D(fileName);
            pMesh3D->UpdateNormal();
            pMesh3D->UnifyPosition(2.f);
            PrimitiveDetectionApp* pPD = dynamic_cast<PrimitiveDetectionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PrimitiveDetectionApp"));
            if (pPD != NULL)
            {
                pPD->SetMesh3D(pMesh3D);
                MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("TestMesh3D", "MyCookTorrance", pMesh3D);
            }
        }
    }

    void PrimitiveDetectionAppUI::RansacMethod(MyGUI::Widget* pSender)
    {
        PrimitiveDetectionApp* pPD = dynamic_cast<PrimitiveDetectionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PrimitiveDetectionApp"));
        pPD->RansacPrimitiveDetection();
    }

}
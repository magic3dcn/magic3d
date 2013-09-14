#include "StdAfx.h"
#include "ReconstructionUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/AppManager.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../Common/RenderSystem.h"
#include "Reconstruction.h"

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
        mRoot.at(0)->findWidget("But_TSDF")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionUI::TSDFExtration);
        mRoot.at(0)->findWidget("But_Fusion")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &ReconstructionUI::PointSetFusion);
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
        static int pcIndex = 0;
        MagicLog << "ReconstructionUI::OpenPointSet" << std::endl;
        std::string fileName;
        MagicCore::ToolKit::GetSingleton()->FileOpenDlg(fileName);
        MagicDGP::Point3DSet* pPointSet = MagicDGP::Parser::ParsePointSet(fileName);
       // pPointSet->UnifyPosition(2.0);
        Reconstruction* pReconApp = dynamic_cast<Reconstruction* >(MagicCore::AppManager::GetSingleton()->GetApp("Reconstruction"));
        if (pReconApp != NULL)
        {
            std::string pcName = pReconApp->AddPoint3DSet(pPointSet);
            
            if (pcIndex == 0)
            {
                MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("refPC", "SimplePoint_Red", pPointSet);
                pcIndex++;
            }
            else
            {
                MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("newPC", "SimplePoint_Green", pPointSet);
            }
        }
    }

    void ReconstructionUI::FilterPointSet(MyGUI::Widget* pSender)
    {
        MagicLog << "ReconstructionUI::FilterPointSet" << std::endl;
        Reconstruction* pReconApp = dynamic_cast<Reconstruction* >(MagicCore::AppManager::GetSingleton()->GetApp("Reconstruction"));
        pReconApp->FilterPointSet();
    }

    void ReconstructionUI::AlignPointSet(MyGUI::Widget* pSender)
    {
        MagicLog << "ReconstructionUI::AlignPointSet" << std::endl;
        Reconstruction* pReconApp = dynamic_cast<Reconstruction* >(MagicCore::AppManager::GetSingleton()->GetApp("Reconstruction"));
        pReconApp->AlignPointSet();
        pReconApp->UpdatePCRendering();
    }

    void ReconstructionUI::TSDFExtration(MyGUI::Widget* pSender)
    {
        MagicLog << "ReconstructionUI::TSDFExtration" << std::endl;
        Reconstruction* pReconApp = dynamic_cast<Reconstruction* >(MagicCore::AppManager::GetSingleton()->GetApp("Reconstruction"));
        pReconApp->TSDFExtraction();
    }

    void ReconstructionUI::BackToHomepage(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }

    void ReconstructionUI::PointSetFusion(MyGUI::Widget* pSender)
    {
        Reconstruction* pReconApp = dynamic_cast<Reconstruction* >(MagicCore::AppManager::GetSingleton()->GetApp("Reconstruction"));
        pReconApp->PointSetFusion();
    }
}
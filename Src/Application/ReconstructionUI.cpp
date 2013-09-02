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
        std::string fileName;
        MagicCore::ToolKit::GetSingleton()->FileOpenDlg(fileName);
        MagicDGP::Parser parser;
        MagicDGP::Point3DSet* pPointSet = parser.ParsePointSet(fileName);
       // pPointSet->UnifyPosition(2.0);
        Reconstruction* pReconApp = dynamic_cast<Reconstruction* >(MagicCore::AppManager::GetSingleton()->GetApp("Reconstruction"));
        if (pReconApp != NULL)
        {
            std::string pcName = pReconApp->AddPoint3DSet(pPointSet);
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet(pPointSet, pcName, "SimplePoint");
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
    }

    void ReconstructionUI::BackToHomepage(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}
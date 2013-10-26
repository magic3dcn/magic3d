////#include "StdAfx.h"
#include "PointSetViewerUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../Common/AppManager.h"
#include "../Common/RenderSystem.h"
#include "PointSetViewer.h"
#include "../DGP/MeshReconstruction.h"//test
#include "../DGP/Sampling.h"

namespace MagicApp
{
    PointSetViewerUI::PointSetViewerUI()
    {
    }

    PointSetViewerUI::~PointSetViewerUI()
    {

    }

    void PointSetViewerUI::Setup()
    {
        MagicLog << "PointSetViewerUI::Setup" << std::endl;
        MagicCore::ResourceManager::GetSingleton()->LoadResource("../../Media/Pointviewer", "FileSystem", "Pointviewer");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("PointViewerLayout.layout");
        mRoot.at(0)->findWidget("But_OpenPointSet")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointSetViewerUI::OpenPointSet);
        mRoot.at(0)->findWidget("But_OpenPointSet")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_SavePointSet")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointSetViewerUI::SavePointSet);
        mRoot.at(0)->findWidget("But_SavePointSet")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(0)->findWidget("But_ReconstructionPointSet")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointSetViewerUI::ReconstrutctPointSet);
        mRoot.at(0)->findWidget("But_ReconstructionPointSet")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(1)->findWidget("But_OpenMesh")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointSetViewerUI::OpenMesh3D);
        mRoot.at(1)->findWidget("But_OpenMesh")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(1)->findWidget("But_SaveMesh")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointSetViewerUI::SaveMesh3D);
        mRoot.at(1)->findWidget("But_SaveMesh")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(1)->findWidget("But_FilterMesh")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointSetViewerUI::FilterMesh3D);
        mRoot.at(1)->findWidget("But_FilterMesh")->castType<MyGUI::Button>()->setSize(86, 87);
        mRoot.at(1)->findWidget("But_BackToHome")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointSetViewerUI::BackToHomepage);
        mRoot.at(1)->findWidget("But_BackToHome")->castType<MyGUI::Button>()->setSize(86, 87);
    }

    void PointSetViewerUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::GetSingleton()->UnloadResource("Pointviewer");
    }

    void PointSetViewerUI::OpenPointSet(MyGUI::Widget* pSender)
    {
        PointSetViewer* pPSViewer = dynamic_cast<PointSetViewer* >(MagicCore::AppManager::GetSingleton()->GetApp("PointSetViewer"));
        if (pPSViewer != NULL)
        {
            if (pPSViewer->ImportPointSet())
            {
                mRoot.at(0)->findWidget("But_ReconstructionPointSet")->castType<MyGUI::Button>()->setVisible(true);
                mRoot.at(0)->findWidget("But_SavePointSet")->castType<MyGUI::Button>()->setVisible(false);
                mRoot.at(1)->findWidget("But_SaveMesh")->castType<MyGUI::Button>()->setVisible(false);
                mRoot.at(1)->findWidget("But_FilterMesh")->castType<MyGUI::Button>()->setVisible(false);
            }
        }
    }

    void PointSetViewerUI::SavePointSet(MyGUI::Widget* pSender)
    {
        PointSetViewer* pPSViewer = dynamic_cast<PointSetViewer* >(MagicCore::AppManager::GetSingleton()->GetApp("PointSetViewer"));
        if (pPSViewer != NULL)
        {
            pPSViewer->ExportPointSet();
        }
    }

    void PointSetViewerUI::ReconstrutctPointSet(MyGUI::Widget* pSender)
    {
        PointSetViewer* pPSViewer = dynamic_cast<PointSetViewer* >(MagicCore::AppManager::GetSingleton()->GetApp("PointSetViewer"));
        if (pPSViewer != NULL)
        {
            if (pPSViewer->ReconstructPointSet())
            {
                mRoot.at(1)->findWidget("But_SaveMesh")->castType<MyGUI::Button>()->setVisible(true);
                mRoot.at(1)->findWidget("But_FilterMesh")->castType<MyGUI::Button>()->setVisible(true);
            }
        }
    }

    void PointSetViewerUI::OpenMesh3D(MyGUI::Widget* pSender)
    {
        PointSetViewer* pPSViewer = dynamic_cast<PointSetViewer* >(MagicCore::AppManager::GetSingleton()->GetApp("PointSetViewer"));
        if (pPSViewer != NULL)
        {
            if (pPSViewer->ImportMesh3D())
            {
                mRoot.at(0)->findWidget("But_ReconstructionPointSet")->castType<MyGUI::Button>()->setVisible(false);
                mRoot.at(0)->findWidget("But_SavePointSet")->castType<MyGUI::Button>()->setVisible(false);
                mRoot.at(1)->findWidget("But_SaveMesh")->castType<MyGUI::Button>()->setVisible(false);
                mRoot.at(1)->findWidget("But_FilterMesh")->castType<MyGUI::Button>()->setVisible(true);
            }
        }
    }

    void PointSetViewerUI::SaveMesh3D(MyGUI::Widget* pSender)
    {
        PointSetViewer* pPSViewer = dynamic_cast<PointSetViewer* >(MagicCore::AppManager::GetSingleton()->GetApp("PointSetViewer"));
        if (pPSViewer != NULL)
        {
            pPSViewer->ExportMesh3D();
        }
    }

    void PointSetViewerUI::FilterMesh3D(MyGUI::Widget* pSender)
    {
        PointSetViewer* pPSViewer = dynamic_cast<PointSetViewer* >(MagicCore::AppManager::GetSingleton()->GetApp("PointSetViewer"));
        if (pPSViewer != NULL)
        {
            if (pPSViewer->FilterMesh3D())
            {
                mRoot.at(1)->findWidget("But_SaveMesh")->castType<MyGUI::Button>()->setVisible(true);
            }
        }
    }

    void PointSetViewerUI::BackToHomepage(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}
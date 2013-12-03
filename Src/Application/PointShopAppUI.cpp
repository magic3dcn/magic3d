#include "PointShopAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../Common/AppManager.h"
#include "PointShopApp.h"

namespace MagicApp
{
    PointShopAppUI::PointShopAppUI()
    {
    }

    PointShopAppUI::~PointShopAppUI()
    {
    }

    void PointShopAppUI::Setup()
    {
        InfoLog << "PointShopAppUI::Setup" << std::endl;
        MagicCore::ResourceManager::LoadResource("../../Media/PointShopApp", "FileSystem", "PointShopApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("PointShopApp.layout");
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::OpenPointSet);
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SavePointSet);
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_CalNormal")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::CalPointSetNormal);
        mRoot.at(0)->findWidget("But_CalNormal")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_FlipNormal")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::FlipPointSetNormal);
        mRoot.at(0)->findWidget("But_FlipNormal")->castType<MyGUI::Button>()->setSize(40, 40);
        mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::FilterPointSet);
        mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SmoothPointSet);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->setSize(40, 40);
        mRoot.at(0)->findWidget("But_Sampling")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SamplePointSet);
        mRoot.at(0)->findWidget("But_Sampling")->castType<MyGUI::Button>()->setSize(40, 40);
        mRoot.at(0)->findWidget("But_Outlier")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::RemoveOutlier);
        mRoot.at(0)->findWidget("But_Outlier")->castType<MyGUI::Button>()->setSize(40, 40);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::Reconstruction);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_AddNoise")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::AddNoise);
        mRoot.at(0)->findWidget("But_AddNoise")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_Select")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SelectPointSet);
        mRoot.at(0)->findWidget("But_Select")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::RectangleSelect);
        mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->setSize(40, 40);
        mRoot.at(0)->findWidget("But_Select_Cycle")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::CycleSelect);
        mRoot.at(0)->findWidget("But_Select_Cycle")->castType<MyGUI::Button>()->setSize(40, 40);
        mRoot.at(0)->findWidget("But_Select_Intelligent")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::IntelligentSelect);
        mRoot.at(0)->findWidget("But_Select_Intelligent")->castType<MyGUI::Button>()->setSize(40, 40);
        mRoot.at(0)->findWidget("But_Deform")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::DeformPointSet);
        mRoot.at(0)->findWidget("But_Deform")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::BackToHome);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->setSize(60, 60);
    }

    void PointShopAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("PointShopApp");
    }

    void PointShopAppUI::OpenPointSet(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            bool hasNormal = false;
            if (pPSA->OpenPointSet(hasNormal))
            {
                mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setVisible(true);
                mRoot.at(0)->findWidget("But_CalNormal")->castType<MyGUI::Button>()->setVisible(true);
                mRoot.at(0)->findWidget("But_FlipNormal")->castType<MyGUI::Button>()->setVisible(true);
                mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->setVisible(true);
                mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->setVisible(hasNormal);
                mRoot.at(0)->findWidget("But_AddNoise")->castType<MyGUI::Button>()->setVisible(true);
                mRoot.at(0)->findWidget("But_Select")->castType<MyGUI::Button>()->setVisible(true);
                mRoot.at(0)->findWidget("But_Deform")->castType<MyGUI::Button>()->setVisible(true);
            }
        }
    }

    void PointShopAppUI::SavePointSet(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->SavePointSet();
        }
    }

    void PointShopAppUI::CalPointSetNormal(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->CalPointSetNormal();
            mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->setVisible(true);
        }
    }

    void PointShopAppUI::FlipPointSetNormal(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->FlipPointSetNormal();
        }
    }

    void PointShopAppUI::FilterPointSet(MyGUI::Widget* pSender)
    {
        bool isVisible = mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->isVisible();
        mRoot.at(0)->findWidget("But_Sampling")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_Outlier")->castType<MyGUI::Button>()->setVisible(!isVisible);
    }

    void PointShopAppUI::SmoothPointSet(MyGUI::Widget* pSender)
    {

    }

    void PointShopAppUI::SamplePointSet(MyGUI::Widget* pSender)
    {

    }

    void PointShopAppUI::RemoveOutlier(MyGUI::Widget* pSender)
    {

    }

    void PointShopAppUI::Reconstruction(MyGUI::Widget* pSender)
    {

    }

    void PointShopAppUI::AddNoise(MyGUI::Widget* pSender)
    {

    }

    void PointShopAppUI::SelectPointSet(MyGUI::Widget* pSender)
    {

    }

    void PointShopAppUI::RectangleSelect(MyGUI::Widget* pSender)
    {

    }

    void PointShopAppUI::CycleSelect(MyGUI::Widget* pSender)
    {

    }

    void PointShopAppUI::IntelligentSelect(MyGUI::Widget* pSender)
    {

    }

    void PointShopAppUI::DeformPointSet(MyGUI::Widget* pSender)
    {

    }

    void PointShopAppUI::BackToHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }

}
#include "PointShopAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../Common/AppManager.h"
#include "../Common/RenderSystem.h"
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
        mRoot.at(0)->findWidget("But_FlipNormal")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::FilterPointSet);
        mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SmoothPointSet);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->setSize(25, 25);
        mRoot.at(0)->findWidget("But_Sampling")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SamplePointSet);
        mRoot.at(0)->findWidget("But_Sampling")->castType<MyGUI::Button>()->setSize(25, 25);
        mRoot.at(0)->findWidget("But_Outlier")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::RemoveOutlier);
        mRoot.at(0)->findWidget("But_Outlier")->castType<MyGUI::Button>()->setSize(25, 25);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::Reconstruction);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_AddNoise")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::AddNoise);
        mRoot.at(0)->findWidget("But_AddNoise")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_Select")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SelectPointSet);
        mRoot.at(0)->findWidget("But_Select")->castType<MyGUI::Button>()->setSize(60, 60);
        mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::RectangleSelect);
        mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->setSize(25, 25);
        mRoot.at(0)->findWidget("But_Select_Cycle")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::CycleSelect);
        mRoot.at(0)->findWidget("But_Select_Cycle")->castType<MyGUI::Button>()->setSize(25, 25);
        mRoot.at(0)->findWidget("But_Select_Intelligent")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::IntelligentSelect);
        mRoot.at(0)->findWidget("But_Select_Intelligent")->castType<MyGUI::Button>()->setSize(25, 25);
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

    void PointShopAppUI::SetupFromPointsetInput(bool hasNormal, int pointNum)
    {
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setEnabled(true);
        mRoot.at(0)->findWidget("But_CalNormal")->castType<MyGUI::Button>()->setEnabled(true);
        mRoot.at(0)->findWidget("But_FlipNormal")->castType<MyGUI::Button>()->setEnabled(hasNormal);
        mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->setEnabled(true);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->setEnabled(hasNormal);
        mRoot.at(0)->findWidget("But_AddNoise")->castType<MyGUI::Button>()->setEnabled(true);
        mRoot.at(0)->findWidget("But_Select")->castType<MyGUI::Button>()->setEnabled(true);
        mRoot.at(0)->findWidget("But_Deform")->castType<MyGUI::Button>()->setEnabled(true);
        std::stringstream ss;
        std::string textString;
        ss << pointNum;
        ss >> textString;
        mRoot.at(0)->findWidget("Edit_SampleNumber")->castType<MyGUI::EditBox>()->setOnlyText(textString);
    }

    void PointShopAppUI::SetProgressBarPosition(int pos)
    {
        mRoot.at(0)->findWidget("Progress_General")->castType<MyGUI::ProgressBar>()->setProgressPosition(pos);
    }

    void PointShopAppUI::SetProgressBarRange(int range)
    {
        mRoot.at(0)->findWidget("Progress_General")->castType<MyGUI::ProgressBar>()->setProgressRange(range);
    }

    void PointShopAppUI::OpenPointSet(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            bool hasNormal = false;
            int pointNum = 0;
            if (pPSA->OpenPointSet(hasNormal, pointNum))
            {
                mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_CalNormal")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_FlipNormal")->castType<MyGUI::Button>()->setEnabled(hasNormal);
                mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->setEnabled(hasNormal);
                mRoot.at(0)->findWidget("But_AddNoise")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_Select")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_Deform")->castType<MyGUI::Button>()->setEnabled(true);

                std::stringstream ss;
                std::string textString;
                ss << pointNum;
                ss >> textString;
                mRoot.at(0)->findWidget("Edit_SampleNumber")->castType<MyGUI::EditBox>()->setOnlyText(textString);
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
            mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->setEnabled(true);
            mRoot.at(0)->findWidget("But_FlipNormal")->castType<MyGUI::Button>()->setEnabled(true);
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
        mRoot.at(0)->findWidget("Edit_SampleNumber")->castType<MyGUI::EditBox>()->setVisible(!isVisible);
    }

    void PointShopAppUI::SmoothPointSet(MyGUI::Widget* pSender)
    {

    }

    void PointShopAppUI::SamplePointSet(MyGUI::Widget* pSender)
    {
        std::string textString = mRoot.at(0)->findWidget("Edit_SampleNumber")->castType<MyGUI::EditBox>()->getOnlyText();
        int sampleNum = std::atoi(textString.c_str());
        if (sampleNum > 0)
        {
            PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
            if (pPSA != NULL)
            {
                pPSA->SamplePointSet(sampleNum);
            }
        }
        DebugLog << "PointShopAppUI::SamplePointSet: " << sampleNum << " " << textString.c_str() << std::endl;
    }

    void PointShopAppUI::RemoveOutlier(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->RemoveOutlier();
        }
    }

    void PointShopAppUI::Reconstruction(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->Reconstruction();
        }
    }

    void PointShopAppUI::AddNoise(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->AddNoise();
        }
    }

    void PointShopAppUI::SelectPointSet(MyGUI::Widget* pSender)
    {
        bool isVisible = mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->isVisible();
        mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_Select_Cycle")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_Select_Intelligent")->castType<MyGUI::Button>()->setVisible(!isVisible);
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
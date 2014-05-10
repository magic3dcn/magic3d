#include "PointShopAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Tool/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../Common/AppManager.h"
#include "../Common/RenderSystem.h"
#include "PointShopApp.h"

namespace MagicApp
{
    PointShopAppUI::PointShopAppUI() :
        mPickIgnoreBack(false)
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
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SavePointSet);
        mRoot.at(0)->findWidget("But_CalNormal")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::CalPointSetNormal);
        mRoot.at(0)->findWidget("But_FlipNormal")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::FlipPointSetNormal);
        mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::FilterPointSet);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SmoothPointSet);
        mRoot.at(0)->findWidget("But_Sampling")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SamplePointSet);
        mRoot.at(0)->findWidget("But_Outlier")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::RemoveOutlier);
        mRoot.at(0)->findWidget("But_Reconstruction")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::Reconstruction);
        mRoot.at(0)->findWidget("But_AddNoise")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::AddNoise);
        mRoot.at(0)->findWidget("But_View")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::ViewPointSet);
        mRoot.at(0)->findWidget("But_Select")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SelectPointSet);
        mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::RectangleSelect);
        mRoot.at(0)->findWidget("But_Select_Cycle")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::CycleSelect);
        mRoot.at(0)->findWidget("But_Select_Clear")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::ClearSelect);
        mRoot.at(0)->findWidget("But_Select_Delete")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::DeleteSelect);
        mRoot.at(0)->findWidget("But_IgnoreBack")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::SetPickIgnoreBack);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::BackToHome);
        mRoot.at(0)->findWidget("But_Contact")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &PointShopAppUI::Contact);
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
        std::stringstream ss;
        std::string textString;
        ss << pointNum;
        ss >> textString;
        mRoot.at(0)->findWidget("Edit_SampleNumber")->castType<MyGUI::EditBox>()->setOnlyText(textString);
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
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->SmoothPointSet();
        }
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

    void PointShopAppUI::ViewPointSet(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->ModelViewer();
        }
    }

    void PointShopAppUI::SelectPointSet(MyGUI::Widget* pSender)
    {
        bool isVisible = mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->isVisible();
        mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_Select_Cycle")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_Select_Clear")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_Select_Delete")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_View")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_IgnoreBack")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("Tex_Back")->castType<MyGUI::TextBox>()->setVisible(!isVisible);
        if (isVisible == true)
        {
            PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
            if (pPSA != NULL)
            {
                pPSA->ModelViewer();
            }
        }
    }

    void PointShopAppUI::RectangleSelect(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->SetPickIgnoreBack(mPickIgnoreBack);
            pPSA->RectangleSelect();
        }
    }

    void PointShopAppUI::CycleSelect(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->SetPickIgnoreBack(mPickIgnoreBack);
            pPSA->CycleSelect();
        }
    }

    void PointShopAppUI::ClearSelect(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->ClearSelect();
        }
    }

    void PointShopAppUI::DeleteSelect(MyGUI::Widget* pSender)
    {
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->DeleteSelcetPoints();
        }
    }

    void PointShopAppUI::SetPickIgnoreBack(MyGUI::Widget* pSender)
    {
        mPickIgnoreBack = !mPickIgnoreBack;
        if (mPickIgnoreBack == true)
        {
            mRoot.at(0)->findWidget("But_IgnoreBack")->castType<MyGUI::Button>()->changeWidgetSkin("But_CheckOn");
        }
        else
        {
            mRoot.at(0)->findWidget("But_IgnoreBack")->castType<MyGUI::Button>()->changeWidgetSkin("But_CheckOff");
        }
    }

    void PointShopAppUI::BackToHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }

    void PointShopAppUI::Contact(MyGUI::Widget* pSender)
    {
        MagicCore::ToolKit::OpenWebsite(std::string("http://magic3dcn.wix.com/magic3d#!form__map/c24vq"));
    }
}
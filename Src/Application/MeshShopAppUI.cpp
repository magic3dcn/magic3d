#include "MeshShopAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../Common/AppManager.h"
#include "MeshShopApp.h"

namespace MagicApp
{
    MeshShopAppUI::MeshShopAppUI()
    {
    }

    MeshShopAppUI::~MeshShopAppUI()
    {
    }

    void MeshShopAppUI::Setup()
    {
        InfoLog << "MeshShopAppUI::Setup" << std::endl;
        MagicCore::ResourceManager::LoadResource("../../Media/MeshShopApp", "FileSystem", "MeshShopApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("MeshShopApp.layout");
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::OpenMesh);
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::SaveMesh);
        mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::FilterMesh);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::SmoothMesh);
        mRoot.at(0)->findWidget("But_Simplify")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::SimplifyMesh);
        mRoot.at(0)->findWidget("But_Outlier")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::RemoveOutlier);
        mRoot.at(0)->findWidget("But_AddNoise")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::AddNoise);
        mRoot.at(0)->findWidget("But_Select")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::SelectMesh);
        mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::RectangleSelect);
        mRoot.at(0)->findWidget("But_Select_Cycle")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::CycleSelect);
        mRoot.at(0)->findWidget("But_Select_Intelligent")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::IntelligentSelect);
        mRoot.at(0)->findWidget("But_Deform")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::DeformMesh);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::BackToHome);
        mRoot.at(0)->findWidget("But_Contact")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MeshShopAppUI::Contact);
    }

    void MeshShopAppUI::SetupFromMeshInput(int vertNum)
    {
        mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setEnabled(true);
        mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->setEnabled(true);
        mRoot.at(0)->findWidget("But_AddNoise")->castType<MyGUI::Button>()->setEnabled(true);
        mRoot.at(0)->findWidget("But_Select")->castType<MyGUI::Button>()->setEnabled(true);
        mRoot.at(0)->findWidget("But_Deform")->castType<MyGUI::Button>()->setEnabled(true);
        std::stringstream ss;
        std::string textString;
        ss << vertNum;
        ss >> textString;
        mRoot.at(0)->findWidget("Edit_SimplifyNumber")->castType<MyGUI::EditBox>()->setOnlyText(textString);
    }

    void MeshShopAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("MeshShopApp");
    }

    void MeshShopAppUI::OpenMesh(MyGUI::Widget* pSender)
    {
        MeshShopApp* pMSA = dynamic_cast<MeshShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MeshShopApp"));
        if (pMSA != NULL)
        {
            int vertNum = 0;
            if (pMSA->OpenMesh(vertNum))
            {
                mRoot.at(0)->findWidget("But_Save")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_Filter")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_AddNoise")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_Select")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_Deform")->castType<MyGUI::Button>()->setEnabled(true);

                std::stringstream ss;
                std::string textString;
                ss << vertNum;
                ss >> textString;
                mRoot.at(0)->findWidget("Edit_SimplifyNumber")->castType<MyGUI::EditBox>()->setOnlyText(textString);
            }
        }
    }

    void MeshShopAppUI::SaveMesh(MyGUI::Widget* pSender)
    {
        MeshShopApp* pMSA = dynamic_cast<MeshShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MeshShopApp"));
        if (pMSA != NULL)
        {
            pMSA->SaveMesh();
        }
    }

    void MeshShopAppUI::FilterMesh(MyGUI::Widget* pSender)
    {
        bool isVisible = mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->isVisible();
        mRoot.at(0)->findWidget("But_Simplify")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_Smooth")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_Outlier")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("Edit_SimplifyNumber")->castType<MyGUI::EditBox>()->setVisible(!isVisible);
    }

    void MeshShopAppUI::SmoothMesh(MyGUI::Widget* pSender)
    {
        MeshShopApp* pMSA = dynamic_cast<MeshShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MeshShopApp"));
        if (pMSA != NULL)
        {
            pMSA->SmoothMesh();
        }
    }

    void MeshShopAppUI::SimplifyMesh(MyGUI::Widget* pSender)
    {

    }

    void MeshShopAppUI::RemoveOutlier(MyGUI::Widget* pSender)
    {
        MeshShopApp* pMSA = dynamic_cast<MeshShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MeshShopApp"));
        if (pMSA != NULL)
        {
            pMSA->RemoveOutlier();
        }
    }

    void MeshShopAppUI::AddNoise(MyGUI::Widget* pSender)
    {
         MeshShopApp* pMSA = dynamic_cast<MeshShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MeshShopApp"));
        if (pMSA != NULL)
        {
            pMSA->AddNoise();
        }
    }

    void MeshShopAppUI::SelectMesh(MyGUI::Widget* pSender)
    {
        bool isVisible = mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->isVisible();
        mRoot.at(0)->findWidget("But_Select_Rectangle")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_Select_Cycle")->castType<MyGUI::Button>()->setVisible(!isVisible);
        mRoot.at(0)->findWidget("But_Select_Intelligent")->castType<MyGUI::Button>()->setVisible(!isVisible);
    }

    void MeshShopAppUI::RectangleSelect(MyGUI::Widget* pSender)
    {

    }

    void MeshShopAppUI::CycleSelect(MyGUI::Widget* pSender)
    {

    }

    void MeshShopAppUI::IntelligentSelect(MyGUI::Widget* pSender)
    {

    }

    void MeshShopAppUI::DeformMesh(MyGUI::Widget* pSender)
    {

    }

    void MeshShopAppUI::BackToHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }

    void MeshShopAppUI::Contact(MyGUI::Widget* pSender)
    {
        MagicCore::ToolKit::OpenWebsite(std::string("http://magic3dcn.wix.com/magic3d#!form__map/c24vq"));
    }
}
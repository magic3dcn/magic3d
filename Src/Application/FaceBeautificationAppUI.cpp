#include "FaceBeautificationAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Tool/LogSystem.h"
#include "../Common/AppManager.h"
#include "../Common/ToolKit.h"
#include "FaceBeautificationApp.h"

namespace MagicApp
{
    FaceBeautificationAppUI::FaceBeautificationAppUI() :
        mpLeftImageTexture(NULL),
        mpMiddleImageTexture(NULL),
        mpRightImageTexture(NULL)
    {
    }

    FaceBeautificationAppUI::~FaceBeautificationAppUI()
    {
        if (mpLeftImageTexture != NULL)
        {
            MyGUI::RenderManager::getInstance().destroyTexture(mpLeftImageTexture);
            mpLeftImageTexture = NULL;
        }
        if (mpMiddleImageTexture != NULL)
        {
            MyGUI::RenderManager::getInstance().destroyTexture(mpMiddleImageTexture);
            mpMiddleImageTexture = NULL;
        }
        if (mpRightImageTexture != NULL)
        {
            MyGUI::RenderManager::getInstance().destroyTexture(mpRightImageTexture);
            mpRightImageTexture = NULL;
        }
    }

    void FaceBeautificationAppUI::Setup()
    {
        InfoLog << "FaceBeautificationAppUI::Setup" << std::endl;
        MagicCore::ResourceManager::LoadResource("../../Media/FaceBeautificationApp", "FileSystem", "FaceBeautificationApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("FaceBeautificationApp.layout");
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::OpenImage);
        mRoot.at(0)->findWidget("But_LoadFeaturePoint")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::LoadFeaturePoint);
        mRoot.at(0)->findWidget("But_MoveFeature")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::MoveFeaturePoint);
        mRoot.at(0)->findWidget("But_AutoAlignFeature")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::AutoMoveFeaturePoint);
        mRoot.at(0)->findWidget("But_AlignToMean")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::AlignToMean);
        mRoot.at(0)->findWidget("But_SaveFeaturePoint")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::SaveFeaturePoint);
        mRoot.at(0)->findWidget("But_OpenReference")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::OpenReferenceImage);
        mRoot.at(0)->findWidget("But_LoadRefFeaturePoint")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::LoadRefFeaturePoint);
        mRoot.at(0)->findWidget("But_Project")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::ProjectRefFeaturePoint);
        mRoot.at(0)->findWidget("But_AlignFeature")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::AlignFeaturePoint);
        mRoot.at(0)->findWidget("But_Deform")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::DeformOriginFace);
        mRoot.at(0)->findWidget("But_DoPCA")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::DoPca);
        mRoot.at(0)->findWidget("But_LoadPCA")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::LoadPca);
        mRoot.at(0)->findWidget("But_ObservePca")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::ObservePca);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::BackHome);
        mRoot.at(0)->findWidget("But_Contact")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::Contact);
    }

    void FaceBeautificationAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("FaceBeautificationApp");
    }

    void FaceBeautificationAppUI::UpdateLeftImage(const cv::Mat& image)
    {
        UpdateImageTexture(&mpLeftImageTexture, image, std::string("LeftImageTexture"));
        MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Left")->castType<MyGUI::ImageBox>();
        pIB->setVisible(true);
        pIB->setImageTexture("LeftImageTexture");
    }

    void FaceBeautificationAppUI::UpdateMiddleImage(const cv::Mat& image)
    {
        UpdateImageTexture(&mpMiddleImageTexture, image, std::string("MiddleImageTexture"));
        MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Middle")->castType<MyGUI::ImageBox>();
        pIB->setVisible(true);
        pIB->setImageTexture("MiddleImageTexture");
    }

    void FaceBeautificationAppUI::UpdateRightImage(const cv::Mat& image)
    {
        UpdateImageTexture(&mpRightImageTexture, image, std::string("RightImageTexture"));
        MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Right")->castType<MyGUI::ImageBox>();
        pIB->setVisible(true);
        pIB->setImageTexture("RightImageTexture");
    }

    void FaceBeautificationAppUI::HideLeftImage(void)
    {
        MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Left")->castType<MyGUI::ImageBox>();
        pIB->setVisible(false);
    }

    void FaceBeautificationAppUI::HideMiddleImage(void)
    {
        MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Middle")->castType<MyGUI::ImageBox>();
        pIB->setVisible(false);
    }

    void FaceBeautificationAppUI::HideRightImage(void)
    {
        MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Right")->castType<MyGUI::ImageBox>();
        pIB->setVisible(false);
    }

    void FaceBeautificationAppUI::UpdateImageTexture(MyGUI::ITexture** pImageTex, const cv::Mat& image, const std::string& texName)
    {
        int texW = image.cols;
        int texH = image.rows;
        if ((*pImageTex) != NULL)
        {
            MyGUI::RenderManager::getInstance().destroyTexture(*pImageTex);
            (*pImageTex) = NULL;
        }
        (*pImageTex) = MyGUI::RenderManager::getInstance().createTexture(texName);
        (*pImageTex)->createManual(texW, texH, MyGUI::TextureUsage::Static | MyGUI::TextureUsage::Write, MyGUI::PixelFormat::R8G8B8A8);
        MyGUI::uint8* pDest = static_cast<MyGUI::uint8*>((*pImageTex)->lock(MyGUI::TextureUsage::Write));
        for (int yid = 0; yid < texH; yid++)
        {
            for (int xid = 0; xid < texW; xid++)
            {
                const unsigned char* pixel = image.ptr(yid, xid);
                *pDest++ = pixel[0];
                *pDest++ = pixel[1];
                *pDest++ = pixel[2];
                *pDest++ = 255;
            }
        }
		(*pImageTex)->unlock();
    }

    void FaceBeautificationAppUI::OpenImage(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            if (pFB->OpenImage())
            {
                mRoot.at(0)->findWidget("But_LoadFeaturePoint")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_SaveFeaturePoint")->castType<MyGUI::Button>()->setEnabled(true);
            }
        }
    }

    void FaceBeautificationAppUI::LoadFeaturePoint(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->LoadFeaturePoint();
            mRoot.at(0)->findWidget("But_MoveFeature")->castType<MyGUI::Button>()->setEnabled(true);
        }
    }

    void FaceBeautificationAppUI::MoveFeaturePoint(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->MoveOriginFeaturePoint();
        }
    }

    void FaceBeautificationAppUI::AutoMoveFeaturePoint(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->AutoMoveOriginFeaturePoint();
        }
    }

    void FaceBeautificationAppUI::AlignToMean(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            //pFB->AlignToMean();
            //pFB->AlignAllToMean();
            //pFB->CalMeanFace();
            pFB->DeformMeanToIndividual();
        }
    }

    void FaceBeautificationAppUI::SaveFeaturePoint(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->SaveFeaturePoint();
        }
    }

    void FaceBeautificationAppUI::OpenReferenceImage(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            if (pFB->OpenRefImage())
            {
                mRoot.at(0)->findWidget("But_LoadRefFeaturePoint")->castType<MyGUI::Button>()->setEnabled(true);
            }
        }
    }

    void FaceBeautificationAppUI::LoadRefFeaturePoint(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->LoadRefFeaturePoint();
        }
    }

    void FaceBeautificationAppUI::ProjectRefFeaturePoint(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->ProjectRefFeatures();
        }
    }

    void FaceBeautificationAppUI::AlignFeaturePoint(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->AlignFeature();
        }
    }

    void FaceBeautificationAppUI::DeformOriginFace(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->DeformOriginFace();
        }
    }

    void FaceBeautificationAppUI::DoPca(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->DoPca();
        }
    }

    void FaceBeautificationAppUI::LoadPca(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->LoadPca();
        }
    }

    void FaceBeautificationAppUI::ObservePca(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->ObservePca();
        }
    }

    void FaceBeautificationAppUI::BackHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }

    void FaceBeautificationAppUI::Contact(MyGUI::Widget* pSender)
    {
        MagicCore::ToolKit::OpenWebsite(std::string("http://magic3dcn.wix.com/magic3d#!form__map/c24vq"));
    }
}

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

    void FaceBeautificationAppUI::Setup(int* maxFaceWidth, int* maxFaceHeight)
    {
        InfoLog << "FaceBeautificationAppUI::Setup" << std::endl;
        *maxFaceWidth = 300;
        *maxFaceHeight = 375;
        MagicCore::ResourceManager::LoadResource("../../Media/FaceBeautificationApp", "FileSystem", "FaceBeautificationApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("FaceBeautificationApp.layout");
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::OpenImage);
        mRoot.at(0)->findWidget("But_EditFeature")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::EditFeaturePoint);
        mRoot.at(0)->findWidget("But_DeformFeature")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::DeformImageFeature);
        mRoot.at(0)->findWidget("But_DeformColor")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::DeformImageColor);
        mRoot.at(0)->findWidget("But_OpenReference")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::OpenReferenceImage);
        mRoot.at(0)->findWidget("But_CalReferenceImage")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::CalReferenceImage);
        mRoot.at(0)->findWidget("But_DoFeaturePCA")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::DoFeaturePca);
        mRoot.at(0)->findWidget("But_DoColorPCA")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::DoColorPca);
        mRoot.at(0)->findWidget("But_DoMixPca")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::DoMixPca);
        mRoot.at(0)->findWidget("But_CalMeanFace")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::CalMeanFace);
        mRoot.at(0)->findWidget("But_DeformFeatureToMeanFace")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::DeformFeatureToMeanFace);
        mRoot.at(0)->findWidget("But_DeformColorToMeanFace")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::DeformColorToMeanFace);
        mRoot.at(0)->findWidget("But_DeformToMeanFace")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceBeautificationAppUI::DeformToMeanFace);
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
                mRoot.at(0)->findWidget("But_EditFeature")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_OpenReference")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_CalReferenceImage")->castType<MyGUI::Button>()->setEnabled(true);
            }
        }
    }

    void FaceBeautificationAppUI::EditFeaturePoint(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->EditFeaturePoint();
        }
    }

    void FaceBeautificationAppUI::DeformImageFeature(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            pFB->DeformImageFeature();
        }
    }

    void FaceBeautificationAppUI::DeformImageColor(MyGUI::Widget* pSender)
    {

    }

    void FaceBeautificationAppUI::OpenReferenceImage(MyGUI::Widget* pSender)
    {
        FaceBeautificationApp* pFB = dynamic_cast<FaceBeautificationApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceBeautificationApp"));
        if (pFB != NULL)
        {
            if (pFB->OpenReferenceImage())
            {
                mRoot.at(0)->findWidget("But_DeformFeature")->castType<MyGUI::Button>()->setEnabled(true);
                mRoot.at(0)->findWidget("But_DeformColor")->castType<MyGUI::Button>()->setEnabled(true);
            }
        }
    }

    void FaceBeautificationAppUI::CalReferenceImage(MyGUI::Widget* pSender)
    {

    }

    void FaceBeautificationAppUI::DoFeaturePca(MyGUI::Widget* pSender)
    {

    }

    void FaceBeautificationAppUI::DoColorPca(MyGUI::Widget* pSender)
    {

    }

    void FaceBeautificationAppUI::DoMixPca(MyGUI::Widget* pSender)
    {

    }

    void FaceBeautificationAppUI::CalMeanFace(MyGUI::Widget* pSender)
    {

    }

    void FaceBeautificationAppUI::DeformFeatureToMeanFace(MyGUI::Widget* pSender)
    {

    }

    void FaceBeautificationAppUI::DeformColorToMeanFace(MyGUI::Widget* pSender)
    {

    }

    void FaceBeautificationAppUI::DeformToMeanFace(MyGUI::Widget* pSender)
    {

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

#include "FaceBeautificationAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Common/LogSystem.h"
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

    }

    void FaceBeautificationAppUI::UpdateRightImage(const cv::Mat& image)
    {

    }

    void FaceBeautificationAppUI::HideLeftImage(void)
    {
        MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Left")->castType<MyGUI::ImageBox>();
        pIB->setVisible(false);
    }

    void FaceBeautificationAppUI::HideMiddleImage(void)
    {

    }

    void FaceBeautificationAppUI::HideRightImage(void)
    {

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
            pFB->OpenImage();
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

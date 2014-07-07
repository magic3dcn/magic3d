#include "FaceFeatureRecognitionAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Tool/LogSystem.h"
#include "../Common/AppManager.h"
#include "FaceFeatureRecognitionApp.h"

namespace MagicApp
{
    FaceFeatureRecognitionAppUI::FaceFeatureRecognitionAppUI() :
        mpImageTexture(NULL)
    {
    }

    FaceFeatureRecognitionAppUI::~FaceFeatureRecognitionAppUI()
    {
    }

    void FaceFeatureRecognitionAppUI::Setup()
    {
        InfoLog << "FaceFeatureRecognitionAppUI::Setup" << std::endl;
        MagicCore::ResourceManager::LoadResource("../../Media/FaceFeatureRecognitionApp", "FileSystem", "FaceFeatureRecognitionApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("FaceFeatureRecognitionApp.layout");
        mRoot.at(0)->findWidget("But_Open")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceFeatureRecognitionAppUI::OpenImage);
        mRoot.at(0)->findWidget("But_BackHome")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &FaceFeatureRecognitionAppUI::BackHome);
    }

    void FaceFeatureRecognitionAppUI::Shutdown()
    {
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("FaceFeatureRecognitionApp");
    }

    void FaceFeatureRecognitionAppUI::UpdateImage(const cv::Mat& image)
    {
        int texW = image.cols;
        int texH = image.rows;
        if (mpImageTexture != NULL)
        {
            MyGUI::RenderManager::getInstance().destroyTexture(mpImageTexture);
            mpImageTexture = NULL;
        }
        mpImageTexture = MyGUI::RenderManager::getInstance().createTexture("ImageTexture");
        mpImageTexture->createManual(texW, texH, MyGUI::TextureUsage::Static | MyGUI::TextureUsage::Write, MyGUI::PixelFormat::R8G8B8A8);
        MyGUI::uint8* pDest = static_cast<MyGUI::uint8*>(mpImageTexture->lock(MyGUI::TextureUsage::Write));
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
		mpImageTexture->unlock();

        MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Canvas")->castType<MyGUI::ImageBox>();
        pIB->setSize(texW, texH);
        pIB->setImageTexture("ImageTexture");
    }

    void FaceFeatureRecognitionAppUI::OpenImage(MyGUI::Widget* pSender)
    {
        FaceFeatureRecognitionApp* pFFR = dynamic_cast<FaceFeatureRecognitionApp* >(MagicCore::AppManager::GetSingleton()->GetApp("FaceFeatureRecognitionApp"));
        if (pFFR != NULL)
        {
            pFFR->OpenImage();
        }
    }

    void FaceFeatureRecognitionAppUI::BackHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}

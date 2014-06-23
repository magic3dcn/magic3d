#include "MachineLearningTestAppUI.h"
#include "../Common/ResourceManager.h"
#include "../Tool/LogSystem.h"
#include "../Common/AppManager.h"
#include "MachineLearningTestApp.h"

namespace MagicApp
{
    MachineLearningTestAppUI::MachineLearningTestAppUI() :
        mpImageTexture(NULL)
    {
    }

    MachineLearningTestAppUI::~MachineLearningTestAppUI()
    {
        if (mpImageTexture != NULL)
        {
            MyGUI::RenderManager::getInstance().destroyTexture(mpImageTexture);
            mpImageTexture = NULL;
        }
    }

    void MachineLearningTestAppUI::Setup()
    {
        InfoLog << "MachineLearningTestAppUI::Setup" << std::endl;
        MagicCore::ResourceManager::LoadResource("../../Media/MachineLearningTestApp", "FileSystem", "MachineLearningTestApp");
        mRoot = MyGUI::LayoutManager::getInstance().loadLayout("MachineLearningTestApp.layout");
        mRoot.at(0)->findWidget("But_DrawPoint")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::DrawPoint);
        mRoot.at(0)->findWidget("But_LearnNaiveBayes")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::LearnNaiveBayes);
        mRoot.at(0)->findWidget("But_TestNaiveBayes")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::TestNaiveBayes);
        mRoot.at(0)->findWidget("But_DecisionBoundaryNaiveBayes")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::DecisionBoundaryNaiveBayes);
        mRoot.at(0)->findWidget("But_LearnSVM")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::LearnSVM);
        mRoot.at(0)->findWidget("But_TestSVM")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::TestSVM);
        mRoot.at(0)->findWidget("But_DecisionBoundarySVM")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::DecisionBoundarySVM);
        mRoot.at(0)->findWidget("But_TestPCA")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::TestPCA);
        mRoot.at(0)->findWidget("But_TestLDA")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::TestLDA);
        mRoot.at(0)->findWidget("But_LearnLR")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::LearnLR);
        mRoot.at(0)->findWidget("But_TestLR")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::TestLR);
        mRoot.at(0)->findWidget("But_DecisionBoundaryLR")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::DecisionBoundaryLR);
        mRoot.at(0)->findWidget("But_Home")->castType<MyGUI::Button>()->eventMouseButtonClick += MyGUI::newDelegate(this, &MachineLearningTestAppUI::BackHome);
        UpdateImageTex(NULL, NULL, 0, NULL, NULL, 0);
    }

    void MachineLearningTestAppUI::Shutdown()
    {
        if (mpImageTexture != NULL)
        {
            MyGUI::RenderManager::getInstance().destroyTexture(mpImageTexture);
            mpImageTexture = NULL;
        }
        MyGUI::LayoutManager::getInstance().unloadLayout(mRoot);
        mRoot.clear();
        MagicCore::ResourceManager::UnloadResource("MachineLearningTestApp");
    }

    void MachineLearningTestAppUI::DrawMarkPoint(const std::vector<double>* dataX, const std::vector<int>* dataY, 
        int texWidth, int texHeight, int pointSize, MyGUI::uint8* pText)
    {
        for (int dataId = 0; dataId < dataY->size(); dataId++)
        {
            int wPos = dataX->at(dataId * 2);
            int hPos = dataX->at(dataId * 2 + 1);
            int hBottom = hPos - pointSize;
            hBottom = hBottom > 0 ? hBottom : 0;
            int hUp = hPos + pointSize;
            hUp = hUp >= texHeight ? texHeight - 1 : hUp;
            int wLeft = wPos - pointSize;
            wLeft = wLeft > 0 ? wLeft : 0;
            int wRight = wPos + pointSize;
            wRight = wRight >= texWidth ? texWidth - 1 : wRight;
            for (int hid = hBottom; hid <= hUp; hid++)
            {
                for (int wid = wLeft; wid <= wRight; wid++)
                {
                    if (dataY->at(dataId) == 0)
                    {
                        pText[(hid * texWidth + wid) * 4 + 0] = 255;
                        pText[(hid * texWidth + wid) * 4 + 1] = 0;
                        pText[(hid * texWidth + wid) * 4 + 2] = 0;
                        pText[(hid * texWidth + wid) * 4 + 3] = 255;
                    }
                    else if (dataY->at(dataId) == 1)
                    {
                        pText[(hid * texWidth + wid) * 4 + 0] = 0;
                        pText[(hid * texWidth + wid) * 4 + 1] = 255;
                        pText[(hid * texWidth + wid) * 4 + 2] = 0;
                        pText[(hid * texWidth + wid) * 4 + 3] = 255;
                    }
                    else if (dataY->at(dataId) == 2)
                    {
                        pText[(hid * texWidth + wid) * 4 + 0] = 0;
                        pText[(hid * texWidth + wid) * 4 + 1] = 0;
                        pText[(hid * texWidth + wid) * 4 + 2] = 255;
                        pText[(hid * texWidth + wid) * 4 + 3] = 255;
                    }
                }
            }
        }
    }

    void MachineLearningTestAppUI::UpdateImageTex(const std::vector<double>* dataX, const std::vector<int>* dataY, int dataSize,
            const std::vector<double>* testDataX, const std::vector<int>* testDataY, int testDataSize)
    {
        MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Texture")->castType<MyGUI::ImageBox>();
        int texW = pIB->getWidth();
        int texH = pIB->getHeight();
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
            int baseIndex = yid * texW * 4;
            for (int xid = 0; xid < texW; xid++)
            {
                pDest[baseIndex + xid * 4 + 0] = 222;
                pDest[baseIndex + xid * 4 + 1] = 222;
                pDest[baseIndex + xid * 4 + 2] = 222;
                pDest[baseIndex + xid * 4 + 3] = 255;
            }
        }
        if (dataX != NULL && dataY != NULL)
        {
            DrawMarkPoint(dataX, dataY, texW, texH, dataSize, pDest);
        }
        if (testDataX != NULL && testDataY != NULL)
        {
            DrawMarkPoint(testDataX, testDataY, texW, texH, testDataSize, pDest);
        }
		mpImageTexture->unlock();
        pIB->setImageTexture("ImageTexture");
    }

    void MachineLearningTestAppUI::DrawPoint(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            pMLTest->DrawPoint();
        }
    }

    void MachineLearningTestAppUI::LearnNaiveBayes(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            pMLTest->LearnNaiveBayes();
        }
    }

    void MachineLearningTestAppUI::TestNaiveBayes(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            pMLTest->TestNaiveBayes();
        }
    }

    void MachineLearningTestAppUI::DecisionBoundaryNaiveBayes(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Texture")->castType<MyGUI::ImageBox>();
            pMLTest->NaiveBayesBoundary(pIB->getWidth(), pIB->getHeight());
        }
    }

    void MachineLearningTestAppUI::LearnSVM(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            pMLTest->LearnSVM();
        }
    }

    void MachineLearningTestAppUI::TestSVM(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            pMLTest->TestSVM();
        }
    }

    void MachineLearningTestAppUI::DecisionBoundarySVM(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Texture")->castType<MyGUI::ImageBox>();
            pMLTest->SVMBoundary(pIB->getWidth(), pIB->getHeight());
        }
    }

    void MachineLearningTestAppUI::LearnLR(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            pMLTest->LearnLR();
        }
    }
        
    void MachineLearningTestAppUI::TestLR(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            pMLTest->TestLR();
        }
    }
        
    void MachineLearningTestAppUI::DecisionBoundaryLR(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            MyGUI::ImageBox* pIB = mRoot.at(0)->findWidget("Image_Texture")->castType<MyGUI::ImageBox>();
            pMLTest->LRBoundary(pIB->getWidth(), pIB->getHeight());
        }
    }

    void MachineLearningTestAppUI::TestPCA(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            pMLTest->TestPCA();
        }
    }

    void MachineLearningTestAppUI::TestLDA(MyGUI::Widget* pSender)
    {
        MachineLearningTestApp* pMLTest = dynamic_cast<MachineLearningTestApp* >(MagicCore::AppManager::GetSingleton()->GetApp("MachineLearningTestApp"));
        if (pMLTest != NULL)
        {
            pMLTest->TestLDA();
        }
    }

    void MachineLearningTestAppUI::BackHome(MyGUI::Widget* pSender)
    {
        MagicCore::AppManager::GetSingleton()->SwitchCurrentApp("Homepage");
    }
}

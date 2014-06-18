#include "MachineLearningTestApp.h"
#include "../Tool/LogSystem.h"
#include "../AppModules/MagicObjectManager.h"
#include "../AppModules/SimpleMLObj.h"
#include "../Common/ToolKit.h"
#include "../MachineLearning/PrincipalComponentAnalysis.h"

namespace MagicApp
{
    MachineLearningTestApp::MachineLearningTestApp() : 
        mpMLObj(NULL),
        mCategoryId(0),
        mMouseMode(MM_View)
    {
    }

    MachineLearningTestApp::~MachineLearningTestApp()
    {
    }

    bool MachineLearningTestApp::Enter(void)
    {
        DebugLog << "Enter MachineLearningTestApp" << std::endl;
        mUI.Setup();
        SetupScene();
        return true;
    }

    bool MachineLearningTestApp::Exit(void)
    {
        DebugLog << "Exit MachineLearningTestApp" << std::endl;
        ShutdownScene();
        mUI.Shutdown();

        return true;
    }

    bool MachineLearningTestApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_Draw_Point)
        {
            int wPos = arg.state.X.abs - 80;
            int hPos = arg.state.Y.abs - 10;
            if (wPos >= 0 & hPos >= 0)
            {
                mpMLObj->InsertTrainingData(wPos, hPos, mCategoryId);
                std::vector<double> dataX;
                std::vector<int> dataY;
                mpMLObj->GetTrainingData(dataX, dataY);
                mUI.UpdateImageTex(&dataX, &dataY, 2, NULL, NULL, 0);
            }
        }
        else if (mMouseMode == MM_Test_NaiveBayes || mMouseMode == MM_Test_SVM)
        {
            int wPos = arg.state.X.abs - 80;
            int hPos = arg.state.Y.abs - 10;
            if (wPos >= 0 && hPos >= 0)
            {
                std::vector<double> dataX;
                std::vector<int> dataY;
                mpMLObj->GetTrainingData(dataX, dataY);
                std::vector<double> testDataX(2);      
                testDataX.at(0) = wPos;
                testDataX.at(1) = hPos;
                std::vector<int> testDataY(1);
                if (mMouseMode == MM_Test_NaiveBayes)
                {
                    testDataY.at(0) = mpMLObj->PredictByNaiveBayes(wPos, hPos);
                }
                else if (mMouseMode == MM_Test_SVM)
                {
                    testDataY.at(0) = mpMLObj->PrediectBySVM(wPos, hPos);
                }
                mUI.UpdateImageTex(&dataX, &dataY, 2, &testDataX, &testDataY, 4);
            }
        }

        return true;
    }

    bool MachineLearningTestApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_S)
        {
            SaveMarkPoints();
        }
        else if (arg.key == OIS::KC_L)
        {
            LoadMarkPoints();
        }
        else
        {
            int keyNum = arg.key - OIS::KC_ESCAPE;
            if (keyNum > 0 && keyNum < 4)
            {
                mCategoryId = keyNum - 1;
            }
        }
        
        return true;
    }

    void MachineLearningTestApp::SetupScene(void)
    {
        if (!(MOMGR->IsObjExist("SimpleMLObj")))
        {
            MOMGR->InsertObj("SimpleMLObj", new SimpleMLObj(2));
        }
        mpMLObj = dynamic_cast<SimpleMLObj*>(MOMGR->GetObj("SimpleMLObj"));
    }

    void MachineLearningTestApp::ShutdownScene(void)
    {

    }

    void MachineLearningTestApp::NaiveBayesBoundary(int width, int height)
    {
        std::vector<double> testDataX(width * height * 2);
        std::vector<int> testDataY(width * height);
        int dataId = 0;
        for (int wid = 0; wid < width; wid++)
        {
            for (int hid = 0; hid < height; hid++)
            {
                testDataX.at(dataId * 2) = wid;
                testDataX.at(dataId * 2 + 1) = hid;
                testDataY.at(dataId) = mpMLObj->PredictByNaiveBayes(wid, hid);
                dataId++;
            }
        }
        mUI.UpdateImageTex(NULL, NULL, 0, &testDataX, &testDataY, 1);
    }

    void MachineLearningTestApp::LoadMarkPoints(void)
    {
        std::string fileName;
        char filterName[] = "Mark Points Files(*.mp)\0*.mp\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpMLObj->Load(fileName);
            std::vector<double> dataX;
            std::vector<int> dataY;
            mpMLObj->GetTrainingData(dataX, dataY);
            mUI.UpdateImageTex(&dataX, &dataY, 2, NULL, NULL, 0);
        }
    }

    void MachineLearningTestApp::SaveMarkPoints(void)
    {
        std::string fileName;
        char filterName[] = "Mark Points Files(*.mp)\0*.mp\0";
        if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
        {
            mpMLObj->Save(fileName);
        }
    }

    void MachineLearningTestApp::DrawPoint(void)
    {
        if (mMouseMode == MM_Draw_Point)
        {
            mMouseMode = MM_View;
        }
        else
        {
            mMouseMode = MM_Draw_Point;
        }
    }

    void MachineLearningTestApp::LearnNaiveBayes(void)
    {
        mpMLObj->LearnNaiveBayes(3);
    }

    void MachineLearningTestApp::TestNaiveBayes(void)
    {
        if (mMouseMode == MM_Test_NaiveBayes)
        {
            mMouseMode = MM_View;
        }
        else
        {
            mMouseMode = MM_Test_NaiveBayes;
        }
    }

    void MachineLearningTestApp::LearnSVM(void)
    {
        double startTime = MagicCore::ToolKit::GetTime();
        mpMLObj->LearnSVM();
        DebugLog << "SVM learn time: " << MagicCore::ToolKit::GetTime() - startTime << std::endl;
    }

    void MachineLearningTestApp::TestSVM(void)
    {
        if (mMouseMode == MM_Test_SVM)
        {
            mMouseMode = MM_View;
        }
        else
        {
            mMouseMode = MM_Test_SVM;
        }
    }

    void MachineLearningTestApp::SVMBoundary(int width, int height)
    {
        std::vector<double> testDataX(width * height * 2);
        std::vector<int> testDataY(width * height);
        int dataId = 0;
        for (int wid = 0; wid < width; wid++)
        {
            for (int hid = 0; hid < height; hid++)
            {
                testDataX.at(dataId * 2) = wid;
                testDataX.at(dataId * 2 + 1) = hid;
                testDataY.at(dataId) = mpMLObj->PrediectBySVM(wid, hid);
                dataId++;
            }
        }
        mUI.UpdateImageTex(NULL, NULL, 0, &testDataX, &testDataY, 1);
    }

    void MachineLearningTestApp::TestPCA(void)
    {
        mpMLObj->LearnPCA();
        std::vector<double> pcaVec = mpMLObj->GetPcaVector();
        double centerX = 465;
        double centerY = 372.5;
        std::vector<double> dataX;
        std::vector<int> dataY;
        mpMLObj->GetTrainingData(dataX, dataY);
        std::vector<double> projDataX(dataX.size());
        int dataSize = dataX.size() / 2;
        for (int dataId = 0; dataId < dataSize; dataId++)
        {
            double len = (dataX.at(dataId * 2) - centerX) * pcaVec.at(0) + (dataX.at(dataId * 2 + 1) - centerY) * pcaVec.at(1);
            projDataX.at(dataId * 2) = centerX + len * pcaVec.at(0);
            projDataX.at(dataId * 2 + 1) = centerY + len * pcaVec.at(1);
        }
        mUI.UpdateImageTex(&dataX, &dataY, 2, &projDataX, &dataY, 1);
    }

    void MachineLearningTestApp::TestLDA(void)
    {
        mpMLObj->LearnLDA();
        std::vector<double> ldaVec = mpMLObj->GetLdaVector();
        double centerX = 465;
        double centerY = 372.5;
        std::vector<double> dataX;
        std::vector<int> dataY;
        mpMLObj->GetTrainingData(dataX, dataY);
        std::vector<double> projDataX(dataX.size());
        int dataSize = dataX.size() / 2;
        for (int dataId = 0; dataId < dataSize; dataId++)
        {
            double len = (dataX.at(dataId * 2) - centerX) * ldaVec.at(0) + (dataX.at(dataId * 2 + 1) - centerY) * ldaVec.at(1);
            projDataX.at(dataId * 2) = centerX + len * ldaVec.at(0);
            projDataX.at(dataId * 2 + 1) = centerY + len * ldaVec.at(1);
        }
        mUI.UpdateImageTex(&dataX, &dataY, 2, &projDataX, &dataY, 1);
    }
}

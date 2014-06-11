#include "MachineLearningTestApp.h"
#include "../Tool/LogSystem.h"
#include "../AppModules/MagicObjectManager.h"
#include "../AppModules/SimpleMLObj.h"

namespace MagicApp
{
    MachineLearningTestApp::MachineLearningTestApp() : 
        mpMLObj(NULL),
        mCategoryId(1),
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
            mpMLObj->InsertTrainingData(wPos, hPos, mCategoryId);
            std::vector<double> dataX;
            std::vector<int> dataY;
            mpMLObj->GetTrainingData(dataX, dataY);
            mUI.UpdateImageTex(&dataX, &dataY, NULL, NULL);
        }

        return true;
    }

    bool MachineLearningTestApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        int keyNum = arg.key - OIS::KC_ESCAPE;
        if (keyNum > 0 && keyNum < 10)
        {
            mCategoryId = keyNum;
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

    void MachineLearningTestApp::TestPoint(void)
    {
        if (mMouseMode == MM_Test_Point)
        {
            mMouseMode = MM_View;
        }
        else
        {
            mMouseMode = MM_Test_Point;
        }
    }
}

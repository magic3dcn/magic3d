#pragma once
#include "../Common/AppBase.h"
#include "MachineLearningTestAppUI.h"

namespace MagicApp
{
    class SimpleMLObj;
    class MachineLearningTestApp : public MagicCore::AppBase
    {
    public:
        enum MouseMode
        {
            MM_View,
            MM_Draw_Point,
            MM_Test_NaiveBayes,
            MM_Test_SVM
        };

        MachineLearningTestApp();
        ~MachineLearningTestApp();

        virtual bool Enter(void);
        virtual bool Exit(void);
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );

        void DrawPoint(void);
        void LearnNaiveBayes(void);
        void TestNaiveBayes(void);
        void NaiveBayesBoundary(int width, int height);
        void LearnSVM(void);
        void TestSVM(void);
        void SVMBoundary(int width, int height);

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void LoadMarkPoints(void);
        void SaveMarkPoints(void);

    private:
        MachineLearningTestAppUI mUI;
        SimpleMLObj* mpMLObj;
        int mCategoryId;
        MouseMode mMouseMode;
    };
}

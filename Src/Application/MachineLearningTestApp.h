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
            MM_Test_Point
        };

        MachineLearningTestApp();
        ~MachineLearningTestApp();

        virtual bool Enter(void);
        virtual bool Exit(void);
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );

        void DrawPoint(void);
        void LearnNaiveBayes(void);
        void TestPoint(void);
        void NaiveBayesBoundary(int width, int height);

    private:
        void SetupScene(void);
        void ShutdownScene(void);

    private:
        MachineLearningTestAppUI mUI;
        SimpleMLObj* mpMLObj;
        int mCategoryId;
        MouseMode mMouseMode;
    };
}

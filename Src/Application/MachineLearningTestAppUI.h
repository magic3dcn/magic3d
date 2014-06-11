#pragma once
#include "MyGUI.h"
#include "opencv2/opencv.hpp"

namespace MagicApp
{
    class MachineLearningTestAppUI
    {
    public:
        MachineLearningTestAppUI();
        ~MachineLearningTestAppUI();

        void Setup();
        void Shutdown();
        void UpdateImageTex(const std::vector<double>* dataX, const std::vector<int>* dataY, 
            const std::vector<double>* testDataX, const std::vector<int>* testDataY);

    private:
        void DrawPoint(MyGUI::Widget* pSender);
        void LearnNaiveBayes(MyGUI::Widget* pSender);
        void TestPoint(MyGUI::Widget* pSender);
        void BackHome(MyGUI::Widget* pSender);

    private:
        MyGUI::VectorWidgetPtr mRoot;
        MyGUI::ITexture* mpImageTexture;
    };
}

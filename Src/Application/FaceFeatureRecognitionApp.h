#pragma once
#include "../Common/AppBase.h"
#include "FaceFeatureRecognitionAppUI.h"
#include <vector>

namespace MagicApp
{
    class Face2DObj;
    class CascadedFaceFeatureDetection;
    class FaceFeatureRecognitionApp : public MagicCore::AppBase
    {
    public:
        enum MouseMode
        {
            MM_View,
            MM_Mark_KeyPoint
        };

        FaceFeatureRecognitionApp();
        ~FaceFeatureRecognitionApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );
        virtual void WindowResized( Ogre::RenderWindow* rw );

        void OpenImage(void);
        void LearnCascadedRegression(void);
        void SaveCascadedRegression(void);
        void LoadCascadedRegression(void);
        
        void OpenTestImage(void);
        void MarkKeyPoint(void);
        void SynthesizeData(int keyX, int keyY);


    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void UpdateDisplayImage(const std::vector<double>* dpsList, const std::vector<double>* fpsList);
        void MarkPointsToImage(cv::Mat& img, const std::vector<double>* markList, 
            unsigned char blue, unsigned char green, unsigned char red, int markWidth);

    private:
        FaceFeatureRecognitionAppUI mUI;
        MouseMode mMouseMode;
        Face2DObj* mpF2DObj;
        CascadedFaceFeatureDetection* mpFfd;
    };

}

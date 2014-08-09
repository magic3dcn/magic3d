#pragma once
#include "../Common/AppBase.h"
#include "FaceFeatureRecognitionAppUI.h"
#include <vector>

namespace MagicApp
{
    class Face2DObj;
    class CascadedFaceFeatureDetection;
    class ShapeFaceFeatureDetection;
    class FaceDetection;
    class FaceFeatureRecognitionApp : public MagicCore::AppBase
    {
    public:
        enum MouseMode
        {
            MM_View,
            MM_Mark_KeyPoint,
            MM_Test_KeyPoint,
            MM_Test_Shape,
            MM_Test_Face_Detection
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

        void ShapeRegression(void);
        void LoadShapeRegression(void);

        void RealTimeFaceDetection(void);
        void RealTimeFaceLoading(void);
        
        void OpenTestImage(void);
        void MarkKeyPoint(void);
        void SynthesizeData(int keyX, int keyY);
        void TestKeyPoint(void);
        void TestShape(void);
        void GenerateFacewareHouseFace(void);
        void GenerateNonFace(void);
        void GenerateNonFaceFromFace(void);
        void GenerateStrongNonFace(void);

        void TransferColorToGray(void);
        void GenerateTrainingFaces(void);

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void UpdateDisplayImage(const std::vector<double>* dpsList, const std::vector<double>* fpsList);
        void MarkPointsToImage(cv::Mat& img, const std::vector<double>* markList, 
            unsigned char blue, unsigned char green, unsigned char red, int markWidth);
        void TestFaceDetection(void); //to do
        void TestFaceDetectionInFile(void);

    private:
        FaceFeatureRecognitionAppUI mUI;
        MouseMode mMouseMode;
        Face2DObj* mpF2DObj;
        CascadedFaceFeatureDetection* mpFfd;
        ShapeFaceFeatureDetection* mpShapeRegression;
        FaceDetection* mpFaceDetection;
    };

}

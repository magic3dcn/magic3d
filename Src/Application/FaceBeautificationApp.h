#pragma once
#include "../Common/AppBase.h"
#include "../Math/HomoMatrix3.h"
#include "FaceBeautificationAppUI.h"
#include "../MachineLearning/PrincipalComponentAnalysis.h"
#include "../AppModules/Face2D.h"
#include <vector>

namespace MagicApp
{
    class Face2DObj;
    class FaceBeautificationApp : public MagicCore::AppBase
    {
    public:
        enum MouseMode
        {
            MM_View,
            MM_Move_Origin_Feature,
            MM_Auto_Align_Feature
        };

        FaceBeautificationApp();
        ~FaceBeautificationApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );
        virtual void WindowResized( Ogre::RenderWindow* rw );

        bool OpenImage(void);
        void EditFeaturePoint(void);
        void AutoAlignFeature(void);
        void DeformImageFeature(void);
        void DeformImageColor(void);
        bool OpenReferenceImage(void);
        bool CalReferenceImage(void);
        void DoFeaturePca(void);
        void DoColorPca(void);
        void DoMixPca(void);
        void CalMeanFace(void);
        void DeformFeatureToMeanFace(void);
        void DeformColorToMeanFace(void);
        void DeformToMeanFace(void);

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void MarkPointsToImage(cv::Mat& img, const std::vector<int>* markList, 
            unsigned char blue, unsigned char green, unsigned char red, int markWidth);
        void UpdateLeftDisplayImage(const std::vector<int>* dpsList, const std::vector<int>* fpsList);
        void UpdateRightDisplayImage(const std::vector<int>* fpsList);
        void UpdateMidDisplayImage(const cv::Mat& img);
        void UpdateMidDisplayImage(const std::vector<int>& markList);
        void ReadImgIndex(std::vector<int>* imgIndex);

    private:
        FaceBeautificationAppUI mUI;
        Face2D mFace2D;
        cv::Mat mLeftDisplayImage;
        cv::Mat mMidDisplayImage;
        cv::Mat mRightDisplayImage;
        bool mFeaturePointSelected;
        MouseMode mMouseMode;
        int mMaxFaceWidth;
        int mMaxFaceHeight;
        std::vector<int> mAutoAlignPoints;
        std::string mFpsPath;
        Face2DObj* mpF2DObj;
    };

    

}

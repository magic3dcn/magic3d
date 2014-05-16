#pragma once
#include "../Common/AppBase.h"
#include "../Math/HomoMatrix3.h"
#include "FaceBeautificationAppUI.h"
#include "../MachineLearning/PrincipalComponentAnalysis.h"
#include "../AppModules/Face2D.h"
#include <vector>

namespace MagicApp
{
    class FaceBeautificationApp : public MagicCore::AppBase
    {
    public:
        enum MouseMode
        {
            MM_View,
            MM_Move_Origin_Feature
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

        //void LoadFeaturePoint(void);
        //void EditOriginFeaturePoint(void);
        //void AutoMoveOriginFeaturePoint(void);
        //void AlignToMean(void);
        //void AlignAllToMean(void);
        /*void CalMeanFace(void);
        void DeformMeanToIndividual(void);
        void SaveFeaturePoint(void);
        bool OpenRefImage(void);
        void LoadRefFeaturePoint(void);
        void AlignFeature(void);
        void DeformOriginFace(void);
        void DoPca(void);
        void LoadPca(void);
        void ObservePca(void);
        void ProjectRefFeatures(void);*/

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void MarkPointsToImage(cv::Mat& img, const std::vector<int>* markList, 
            unsigned char blue, unsigned char green, unsigned char red, int markWidth);
        void UpdateLeftDisplayImage(const std::vector<int>* dpsList, const std::vector<int>* fpsList);
        //void UpdateMidDisplayImage(const std::vector<int>* markIndex);
        //void UpdateRightDisplayImage(const cv::Mat& img, const std::vector<int>* leftMarkIndex, 
        //    const std::vector<int>* midMarkIndex, const MagicMath::HomoMatrix3& midTransform);
        //void UpdateRightDisplayImage(const std::vector<FaceFeaturePoint*>& faceFeatureList, 
        //    const std::vector<MagicMath::HomoMatrix3*>& transformList);
        //void UpdateRightDisplayImage(const std::vector<int>& markIndex);
        //cv::Mat ResizeInputImageToCanvas(const cv::Mat& img) const;

    private:
        FaceBeautificationAppUI mUI;
        Face2D mFace2D;
        //cv::Mat mImage;
        cv::Mat mLeftDisplayImage;
        //cv::Mat mRefImage;
        cv::Mat mMidDisplayImage;
        cv::Mat mRightDisplayImage;
        //FaceFeaturePoint mOriginFPs;
        //FaceFeaturePoint mRefFPs;
        //MagicMath::HomoMatrix3 mRefFPTranform;
        bool mFeaturePointSelected;
        MouseMode mMouseMode;
        int mMaxFaceWidth;
        int mMaxFaceHeight;
        std::string mFpsPath;
        //MagicML::PrincipalComponentAnalysis mPca;
        //ASM
        //std::vector<cv::Mat*> mTrainingImages;
        //std::vector<FaceFeaturePoint*> mTrainingTransforms;
    };

    

}

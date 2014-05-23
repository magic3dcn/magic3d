#pragma once
#include "../Common/AppBase.h"
#include "VisionShopAppUI.h"
#include "opencv2/opencv.hpp"
#include "../DGP/PointCloud3D.h"
#include "../DGP/ViewTool.h"

namespace MagicApp
{
    class VisionShopApp : public MagicCore::AppBase
    {
    public:
        enum MouseMode
        {
            MM_View,
            MM_Paint_Front,
            MM_Paint_Back,
            MM_Deformation
        };

        VisionShopApp();
        ~VisionShopApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );
        virtual void WindowResized( Ogre::RenderWindow* rw );

        bool OpenImage(int& w, int& h);
        void SaveImage(void);
        bool SwitchDisplayMode(void);
        void ImageResizing(int w, int h);
        void FastImageResizing(int w, int h);
        void ImageGradient(void);
        void CannyEdgeDetection(int threshold);
        void DoGBandSaliencyDetection(void);
        void BrushFront(void);
        void BrushBack(void);
        void SegmentImageDo(void);
        void ImageClustering(int k);
        void Deformation(void);

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void UpdateAuxiliaryData(void);
        void UpdateDisplayImage(const cv::Mat& markImg);
        void UpdateDisplayImage(const std::vector<int>& markIndex);
        void UpdatePointSet(void);
        void Display(void);
        void SetDefaultParameter(void);
        cv::Mat ResizeToViewSuit(const cv::Mat& img) const;

    private:
        VisionShopAppUI mUI;
        MouseMode mMouseMode;
        cv::Mat mImage;
        cv::Mat mMarkImage;
        cv::Mat mDisplayImage;
        MagicDGP::Point3DSet* mpPointSet;
        bool mIsPointSetMode;
        bool mIsNewImage;
        MagicDGP::ViewTool mViewTool;
        std::vector<int> mDeformationMarks;
        bool mIsDeformMarkSelected;
        int mSelectedDeformMarkIndex;
    };
}

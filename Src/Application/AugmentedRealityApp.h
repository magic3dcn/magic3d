#pragma once
#include "../Common/AppBase.h"
#include "AugmentedRealityAppUI.h"
#include "../Common/MagicOgre.h"
//#include "OgreTexture.h"
//#include "OgreMaterial.h"
//#include "OgreRenderWindow.h"
//#include "OgreRectangle2D.h"
#include "opencv2/opencv.hpp"

namespace MagicApp
{
    class AugmentedRealityApp  : public MagicCore::AppBase
    {
    public:
        AugmentedRealityApp();
        ~AugmentedRealityApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );
        virtual void WindowResized( Ogre::RenderWindow* rw );

        bool OpenVideo();
        void SetFramePosition(int pos);
        void CaptureFrame();
        void ClearCapture();
        void NextImage();
        void DetectCorners();

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void ResetParameter(void);
        void UpdateImageToCanvas(cv::Mat& image);
        void UpdateCanvas(void);
        void UpdateCanvasSize(int winW, int winH, int videoW, int videoH);

        void GetValidImages();

    private:
        AugmentedRealityAppUI mUI;
        Ogre::Rectangle2D* mpVideoCanvas;
        Ogre::TexturePtr mpVCTex;
        Ogre::MaterialPtr mpVCMat;
        int mTexWidth;
        int mTexHeight;
        cv::VideoCapture mVideoCapture;
        bool mIsUpdateVideoCanvas;
        int mVideoWidth;
        int mVideoHeight;
        float mOneFrameTime;
        float mTimeAccumulate;
        int mCurrentFrame;
        int mFrameCount;
        bool mIsCaptureFrame;
        std::vector<cv::Mat> mSelectedImages;
        std::vector<int> mSelectedIndex;
        int mDisplayIndex;
        int mImageNeedNumber;
    };

}
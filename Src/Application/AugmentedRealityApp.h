#pragma once
#include "../Common/AppBase.h"
#include "AugmentedRealityAppUI.h"
#include "../DGP/Mesh3D.h"
#include "OgreTexture.h"
#include "OgreMaterial.h"
#include "OgreRenderWindow.h"
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

        bool OpenVideo();
        void SetFramePosition(int pos);
        void CaptureFrame();
        void ClearCapture();
        void NextImage();
        void DetectCorners();

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void SetupBackboard();
        void UpdateCameraParm();
        void ResetParameter(void);
        void UpdateImageToCanvas(cv::Mat& image);
        void UpdateCanvas(void);

        void GetValidImages();

    private:
        AugmentedRealityAppUI mUI;
        Ogre::TexturePtr mpVCTex;
        Ogre::MaterialPtr mpVCMat;
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
        float mCameraFov;
        MagicDGP::LightMesh3D* mpBackboardMesh;
        float mBackboardDepth;
    };

}
#pragma once
#include "../Common/AppBase.h"
#include "VideoRecordingAppUI.h"
#include "Ogre.h"
#include "opencv2/opencv.hpp"

namespace MagicApp
{
    class VideoRecordingApp : public MagicCore::AppBase
    {
    public:
        VideoRecordingApp();
        ~VideoRecordingApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual void WindowResized( Ogre::RenderWindow* rw );

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void UpdateCanvas(float timeElapsed);
        void UpdateCanvasSize(int winW, int winH, int videoW, int videoH);

    private:
        VideoRecordingAppUI mUI;
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
        cv::VideoWriter mVideoWriter;
        bool mIsRecording;
    };

}
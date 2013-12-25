#pragma once
#include "../Common/AppBase.h"
#include "AugmentedRealityAppUI.h"
#include "Ogre.h"
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

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void UpdateCanvas(void);
        void UpdateCanvasSize(int winW, int winH, int videoW, int videoH);

    private:
        AugmentedRealityAppUI mUI;
        Ogre::Rectangle2D* mpVideoCanvas;
        Ogre::TexturePtr mpVCTex;
        Ogre::MaterialPtr mpVCMat;
        int mVCWidth;
        int mVCHeight;
        cv::VideoCapture mVideoCapture;
        bool mIsUpdateVideoCanvas;
    };

}
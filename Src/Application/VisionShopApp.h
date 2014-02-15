#pragma once
#include "../Common/AppBase.h"
#include "VisionShopAppUI.h"
#include "opencv2/opencv.hpp"

namespace MagicApp
{
    class VisionShopApp : public MagicCore::AppBase
    {
    public:
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
        void ImageResizing(int w, int h);
        cv::Mat ResizeToViewSuit(const cv::Mat& img) const;

    private:
        void SetupScene(void);
        void ShutdownScene(void);

    private:
        VisionShopAppUI mUI;
        cv::Mat mImage;
    };
}

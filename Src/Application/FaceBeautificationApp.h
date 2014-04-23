#pragma once
#include "../Common/AppBase.h"
#include "FaceBeautificationAppUI.h"
#include <vector>

namespace MagicApp
{
    class FaceFeaturePoint
    {
    public:
        enum FeatureType
        {
            FT_None = 0,
            FT_Left_Eye,
            FT_Right_Eye,
            FT_Mouse,
            FT_Nose,
            FT_Border
        };

        FaceFeaturePoint();
        
        void Load();
        void Save();
        bool Select(int hid, int wid);
        void MoveTo(int hid, int wid);
        void Get(std::vector<int>& posList);

        ~FaceFeaturePoint();

    private:
        int mSelectIndex;
        FeatureType mSelectType;
        std::vector<int> mLeftEyeFPs;
        std::vector<int> mRightEyeFPs;
        std::vector<int> mNoseFPs;
        std::vector<int> mMouseFPs;
        std::vector<int> mBorderFPs;
    };

    class FaceBeautificationApp : public MagicCore::AppBase
    {
    public:
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
        void LoadFeaturePoint();
        void SaveFeaturePoint();

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void UpdateLeftDisplayImage(std::vector<int>* markIndex);

    private:
        FaceBeautificationAppUI mUI;
        cv::Mat mImage;
        cv::Mat mLeftDisplayImage;
        FaceFeaturePoint mOriginFPs;
    };

    

}

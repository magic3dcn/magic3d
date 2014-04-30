#pragma once
#include "../Common/AppBase.h"
#include "../Math/HomoMatrix3.h"
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
            FT_Left_Brow,
            FT_Right_Brow,
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
        void MoveDelta(int deltaH, int deltaW);
        void GetDPs(std::vector<int>& posList); // h, w
        void GetFPs(std::vector<int>& posList);
        void GetTransform(double& posX, double& posY, double& dirX, double& dirY, double& scale);

        ~FaceFeaturePoint();

    private:
        void UpdateDPs();
        void ConstructOneDPs(const std::vector<int>& fps, bool isClosed, int addSize, std::vector<int>& dps);

    private:
        int mSelectIndex;
        FeatureType mSelectType;
        std::vector<int> mLeftBrowFPs;
        std::vector<int> mLeftBrowDPs;
        std::vector<int> mRightBrowFPs;
        std::vector<int> mRightBrowDPs;
        std::vector<int> mLeftEyeFPs;
        std::vector<int> mLeftEyeDPs;
        std::vector<int> mRightEyeFPs;
        std::vector<int> mRightEyeDPs;
        std::vector<int> mNoseFPs;
        std::vector<int> mNoseDPs;
        std::vector<int> mMouseFPs;
        std::vector<int> mMouseDPs;
        std::vector<int> mBorderFPs;
        std::vector<int> mBorderDPs;
    };

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
        void LoadFeaturePoint(void);
        void MoveOriginFeaturePoint(void);
        void SaveFeaturePoint(void);
        bool OpenRefImage(void);
        void LoadRefFeaturePoint(void);
        void AlignFeature(void);
        void DeformOriginFace(void);

    private:
        void SetupScene(void);
        void ShutdownScene(void);
        void UpdateLeftDisplayImage(const std::vector<int>* markIndex, const std::vector<int>* featureIndex);
        void UpdateMidDisplayImage(const std::vector<int>* markIndex);
        void UpdateRightDisplayImage(const cv::Mat& img, const std::vector<int>* leftMarkIndex, 
            const std::vector<int>* midMarkIndex, const MagicMath::HomoMatrix3& midTransform);
        cv::Mat ResizeInputImageToCanvas(const cv::Mat& img) const;

    private:
        FaceBeautificationAppUI mUI;
        cv::Mat mImage;
        cv::Mat mLeftDisplayImage;
        cv::Mat mRefImage;
        cv::Mat mMidDisplayImage;
        cv::Mat mRightDisplayImage;
        FaceFeaturePoint mOriginFPs;
        FaceFeaturePoint mRefFPs;
        MagicMath::HomoMatrix3 mRefFPTranform;
        bool mFeaturePointSelected;
        MouseMode mMouseMode;
    };

    

}

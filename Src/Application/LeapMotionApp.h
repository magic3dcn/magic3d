#pragma once
#include "Leap.h"
#include "../Common/AppBase.h"
#include "LeapMotionAppUI.h"
#include "../DGP/Mesh3D.h"
#include "../Tool/ViewTool.h"

namespace MagicApp
{
    class LeapMotionApp : public MagicCore::AppBase, public Leap::Listener
    {
    public:
        LeapMotionApp();
        virtual ~LeapMotionApp();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
        virtual bool KeyPressed( const OIS::KeyEvent &arg );

        virtual void onInit(const Leap::Controller& controller);
        virtual void onConnect(const Leap::Controller& controller);
        virtual void onDisconnect(const Leap::Controller& controller);
        virtual void onExit(const Leap::Controller& controller);
        virtual void onFrame(const Leap::Controller& controller);
        virtual void onFocusGained(const Leap::Controller& controller);
        virtual void onFocusLost(const Leap::Controller& controller);

        void OpenMesh3D();

    private:
        void SetupScene(void);
        void ShutdownScene(void);

    private:
        LeapMotionAppUI mUI;
        MagicTool::ViewTool mViewTool;
        MagicDGP::Mesh3D* mpMesh;
        Leap::Controller mLeapCtrl;
        Leap::Frame mLastFrame;
        bool mLeapMotinOn;
    };


}
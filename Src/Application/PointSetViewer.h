#pragma once
#include "../Common/AppBase.h"
#include "../Tool/ViewTool.h"

namespace MagicApp
{
    class PointSetViewer : public MagicCore::AppBase
    {
    public: 
        PointSetViewer();
        virtual ~PointSetViewer();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);
        virtual bool Pause(void);
        virtual bool Resume(void);
        virtual bool MouseMoved( const OIS::MouseEvent &arg );
        virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    private:
        void SetupScene(void);
        
    private:
        MagicTool::ViewTool mViewTool;
    };
}
#include "stdafx.h"
#include "VisionShopApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"

namespace MagicApp
{
    VisionShopApp::VisionShopApp() :
        mUI()
    {
    }

    VisionShopApp::~VisionShopApp()
    {
    }

    bool VisionShopApp::Enter(void)
    {
        InfoLog << "Enter VisionShopApp" << std::endl;
        mUI.Setup();
        SetupScene();

        return true;
    }

    bool VisionShopApp::Update(float timeElapsed)
    {
        return true;
    }

    bool VisionShopApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();

        return true;
    }

    bool VisionShopApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        return true;
    }

    bool VisionShopApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool VisionShopApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool VisionShopApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        return true;
    }

    void VisionShopApp::SetupScene(void)
    {

    }

    void VisionShopApp::ShutdownScene(void)
    {

    }
}

#include "AugmentedRealityApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"

namespace MagicApp
{
    AugmentedRealityApp::AugmentedRealityApp()
    {
    }

    AugmentedRealityApp::~AugmentedRealityApp()
    {
    }

    bool AugmentedRealityApp::Enter(void)
    {
        InfoLog << "Enter AugmentedRealityApp" << std::endl; 
        mUI.Setup();
        SetupScene();
        return true;
    }

    bool AugmentedRealityApp::Update(float timeElapsed)
    {
        return true;
    }
    
    bool AugmentedRealityApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();
        return true;
    }

    bool AugmentedRealityApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        return true;
    }

    bool AugmentedRealityApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool AugmentedRealityApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool AugmentedRealityApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        return true;
    }

    void AugmentedRealityApp::SetupScene(void)
    {

    }

    void AugmentedRealityApp::ShutdownScene(void)
    {

    }
}
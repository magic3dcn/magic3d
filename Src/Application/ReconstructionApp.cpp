#include "ReconstructionApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"

namespace MagicApp
{
    ReconstructionApp::ReconstructionApp() :
        mIsDeviceOpen(false),
        mpPointSet(NULL),
        mpMesh(NULL)
    {
    }

    ReconstructionApp::~ReconstructionApp()
    {
    }

    bool ReconstructionApp::Enter(void)
    {
        MagicLog << "Enter ReconstructionApp" << std::endl;
        mUI.Setup();
        SetupRenderScene();

        return true;
    }

    bool ReconstructionApp::Update(float timeElapsed)
    {
        if (mIsDeviceOpen)
        {
            UpdateScannerDisplay();
        }
        return true;
    }

    bool ReconstructionApp::Exit(void)
    {
        ReleaseRenderScene();
        mUI.Shutdown();

        return true;
    }

    void ReconstructionApp::SetupRenderScene(void)
    {
        MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPosition(0, 0, 500);
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light* frontLight = pSceneMgr->createLight("frontLight");
        frontLight->setPosition(0, 0, 500);
        frontLight->setDiffuseColour(0.8, 0.8, 0.8);
        frontLight->setSpecularColour(0.5, 0.5, 0.5);
    }

    void ReconstructionApp::ReleaseRenderScene(void)
    {
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->destroyLight("frontLight");
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
    }

    bool ReconstructionApp::SetupDevice()
    {
        if (MagicCore::ToolKit::GetSingleton()->IsONIInitialized() == false)
        {
            openni::Status rc = openni::OpenNI::initialize();
            if (rc != openni::STATUS_OK)
            {
                MagicLog << "OpenNI initialize failed: " << openni::OpenNI::getExtendedError() << std::endl;
                return false;
            }
            else
            {
                MagicLog << "OpenNI initialize succeed" << std::endl;
                MagicCore::ToolKit::GetSingleton()->SetONIInitialized(true);
            }
        }

        return true;
    }

    void ReconstructionApp::ReleaseDevice()
    {

    }

    void ReconstructionApp::UpdateScannerDisplay()
    {

    }

    bool ReconstructionApp::OpenSceneRecord()
    {
        return true;
    }

    void ReconstructionApp::SetTimeStart()
    {

    }

    void ReconstructionApp::SetTimeEnd()
    {

    }

    void ReconstructionApp::ChangeLeftRange(int rel)
    {

    }

    void ReconstructionApp::ChangeRightRange(int rel)
    {

    }

    void ReconstructionApp::ChangeTopRange(int rel)
    {

    }

    void ReconstructionApp::ChangeDownRange(int rel)
    {

    }

    void ReconstructionApp::ChangeFrontRange(int rel)
    {

    }

    void ReconstructionApp::ChangeBackRange(int rel)
    {

    }

    void ReconstructionApp::StartRegistration()
    {
        mUI.StartPostProcess();
    }

    bool ReconstructionApp::SavePointSet()
    {
        return true;
    }

    bool ReconstructionApp::ReconstructPointSet()
    {
        return true;
    }

    bool ReconstructionApp::SaveMesh3D()
    {
        return true;
    }

    void ReconstructionApp::SmoothMesh3D()
    {

    }
}
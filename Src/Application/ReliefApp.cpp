#include "ReliefApp.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../DGP/Relief.h"

namespace MagicApp
{
    ReliefApp::ReliefApp() : 
        mpLightMesh(NULL)
    {
    }

    ReliefApp::~ReliefApp()
    {
        if (mpLightMesh != NULL)
        {
            delete mpLightMesh;
            mpLightMesh = NULL;
        }
    }

    bool ReliefApp::Enter(void)
    {
        InfoLog << "Enter ReliefApp" << std::endl;
        mUI.Setup();
        SetupScene();
        return true;
    }

    bool ReliefApp::Update(float timeElapsed)
    {
        return true;
    }

    bool ReliefApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();
        return true;
    }

    bool ReliefApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        mViewTool.MouseMoved(arg);
        return true;
    }

    bool ReliefApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        mViewTool.MousePressed(arg);
        return true;
    }

    bool ReliefApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool ReliefApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_V && mpLightMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_POINTS);
        }
        if (arg.key == OIS::KC_E && mpLightMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_WIREFRAME);
        }
        if (arg.key == OIS::KC_F && mpLightMesh !=NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPolygonMode(Ogre::PolygonMode::PM_SOLID);
        }
        return true;
    }

    void ReliefApp::SetupScene(void)
    {
        InfoLog << "ReliefApp::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light*  sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(0, 0, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void ReliefApp::ShutdownScene(void)
    {
        InfoLog << "ReliefApp::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("RenderMesh");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetToInitialState();
    }

    bool ReliefApp::ImportMesh3D()
    {
        std::string fileName;
        char filterName[] = "OBJ Files(*.obj)\0*.obj\0STL Files(*.stl)\0*.stl\0OFF Files(*.off)\0*.off\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            MagicDGP::LightMesh3D* pLightMesh = MagicDGP::Parser::ParseLightMesh3D(fileName);
            if (pLightMesh != NULL)
            {
                pLightMesh->UnifyPosition(2.0);
                pLightMesh->UpdateNormal();
                if (mpLightMesh != NULL)
                {
                    delete mpLightMesh;
                }
                mpLightMesh = pLightMesh;
                MagicCore::RenderSystem::GetSingleton()->RenderLightMesh3D("RenderMesh", "MyCookTorrance", mpLightMesh);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    void ReliefApp::GenerateRelief()
    {
        //Get depth data
        Ogre::TexturePtr depthTex = Ogre::TextureManager::getSingleton().createManual(  
            "DepthTexture",      // name   
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,  
            Ogre::TEX_TYPE_2D,   // type   
            512,  // width   
            512,  // height   
            0,                   // number of mipmaps   
            //Ogre::PF_B8G8R8A8,   // pixel format
            Ogre::PF_FLOAT32_R,
            Ogre::TU_RENDERTARGET
            ); 
        Ogre::RenderTarget* pTarget = depthTex->getBuffer()->getRenderTarget();
        Ogre::Camera* pOrthCam = MagicCore::RenderSystem::GetSingleton()->GetMainCamera();
        pOrthCam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
        pOrthCam->setOrthoWindow(3, 3);
        pOrthCam->setPosition(0, 0, 3);
        pOrthCam->lookAt(0, 0, 0);
        pOrthCam->setAspectRatio(1.0);
        pOrthCam->setNearClipDistance(0.5);
        pOrthCam->setFarClipDistance(5);
        Ogre::Viewport* pViewport = pTarget->addViewport(pOrthCam);
        pViewport->setDimensions(0, 0, 1, 1);
        MagicCore::RenderSystem::GetSingleton()->RenderLightMesh3D("RenderMesh", "Depth", mpLightMesh);
        MagicCore::RenderSystem::GetSingleton()->Update();
        Ogre::Image img;
        depthTex->convertToImage(img);
        std::vector<MagicDGP::Real> heightField(512 * 512);
        for(int x = 0; x < 512; x++)  
        {  
            for(int y = 0; y < 512; y++)  
            {
                heightField.at(x * 512 + y) = (img.getColourAt(x, 511 - y, 0))[1];
            }
        }
        Ogre::TextureManager::getSingleton().remove("DepthTexture");
        //
        MagicDGP::LightMesh3D* pReliefMesh = MagicDGP::ReliefGeneration::PlaneReliefFromHeightField(heightField, 511, 511);
        //MagicDGP::LightMesh3D* pReliefMesh = MagicDGP::ReliefGeneration::CylinderReliefFromHeightField(heightField, 511, 511);
        if (pReliefMesh != NULL)
        {
            delete mpLightMesh;
            mpLightMesh = pReliefMesh;
            mpLightMesh->UnifyPosition(2);
            mpLightMesh->UpdateNormal();
        }
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        MagicCore::RenderSystem::GetSingleton()->RenderLightMesh3D("RenderMesh", "MyCookTorrance", mpLightMesh);
    }

    void ReliefApp::ExportReliefMesh()
    {
        if (mpLightMesh != NULL)
        {
            std::string fileName;
            char filterName[] = "Support format(*.obj, *.stl, *.off)\0*.*\0";
            if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
            {
                MagicDGP::Parser::ExportLightMesh3D(fileName, mpLightMesh);
            }
        }
    }

}
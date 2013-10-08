#include "LeapMotionApp.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../Common/RenderSystem.h"

namespace MagicApp
{
    LeapMotionApp::LeapMotionApp() :
        mpMesh(NULL),
        mLeapMotinOn(false)
    {
    }

    LeapMotionApp::~LeapMotionApp()
    {
        if (mpMesh != NULL)
        {
            delete mpMesh;
            mpMesh = NULL;
        }
        if (mLeapMotinOn)
        {
            mLeapCtrl.removeListener(*this);
        }
    }

    bool LeapMotionApp::Enter(void)
    {
        MagicLog << "LeapMotionApp::Enter" << std::endl;
        mUI.Setup();
        SetupScene();

        return true;
    }

    bool LeapMotionApp::Update(float timeElapsed)
    {
        return true;
    }

    bool LeapMotionApp::Exit(void)
    {
        MagicLog << "LeapMotionApp::Exit" << std::endl;
        mUI.Shutdown();
        ShutdownScene();

        if (mLeapMotinOn)
        {
            mLeapCtrl.removeListener(*this);
        }
        return true;
    }

    bool LeapMotionApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        if (!mLeapMotinOn)
        {
            mViewTool.MouseMoved(arg);
        }
        return true;
    }

    bool LeapMotionApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (!mLeapMotinOn)
        {
            mViewTool.MousePressed(arg);
        }
        return true;
    }

    bool LeapMotionApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_L)
        {
            if (!mLeapMotinOn)
            {
                mLeapCtrl.addListener(*this);
                mLeapCtrl.enableGesture(Leap::Gesture::TYPE_SWIPE);
                mLeapMotinOn = true;
            }
        }
        if (arg.key == OIS::KC_M)
        {
            if (mLeapMotinOn)
            {
                mLeapCtrl.removeListener(*this);
                mLeapMotinOn = false;
            }
        }

        return true;
    }

    void LeapMotionApp::onInit(const Leap::Controller& controller)
    {
        MagicLog << "LeapMotionApp::onInit" << std::endl;
    }

    void LeapMotionApp::onConnect(const Leap::Controller& controller)
    {
        MagicLog << "LeapMotionApp::onConnect" << std::endl;
        mLastFrame = controller.frame();
    }

    void LeapMotionApp::onDisconnect(const Leap::Controller& controller)
    {
        MagicLog << "LeapMotionApp::onDisconnect" << std::endl;
    }

    void LeapMotionApp::onExit(const Leap::Controller& controller)
    {
        MagicLog << "LeapMotionApp::onExit" << std::endl;
    }

    void LeapMotionApp::onFrame(const Leap::Controller& controller)
    {
        //MagicLog << "LeapMotionApp::onFrame" << std::endl;
        const Leap::Frame frame = controller.frame();
        /*MagicLog << "Frame id: " << frame.id() << " hands: " << frame.hands().count() << " fingers: "
            << frame.fingers().count() << " tools: " << frame.tools().count() << " gestures: "
            << frame.gestures().count() << std::endl;*/
        const Leap::Vector translate = frame.translation(mLastFrame);
        float translateProb = frame.translationProbability(mLastFrame);
        const Leap::Vector rotateAxis = frame.rotationAxis(mLastFrame);
        float rotateAngle = frame.rotationAngle(mLastFrame);
        float rotateProb = frame.rotationProbability(mLastFrame);
        float scale = frame.scaleFactor(mLastFrame);
        float scaleProb = frame.scaleProbability(mLastFrame);
        /*MagicLog << "  translate: " << translate.x << " " << translate.y << " " << translate.z << " " << translateProb << std::endl;
        MagicLog << "  rotate: " << rotateAxis.x << " " << rotateAxis.y << " " << rotateAxis.z << " " << rotateAngle << " " << rotateProb << std::endl;
        MagicLog << "  scale: " << scale << " " << scaleProb << std::endl;*/
        //MagicLog << std::endl;
        bool isSwipe = false;
        Leap::GestureList gestList = frame.gestures();
        if (!gestList.isEmpty())
        {
            for (int i = 0; i < gestList.count(); i++)
            {
                if (gestList[i].type() == Leap::Gesture::TYPE_SWIPE)
                {
                    isSwipe = true;
                    MagicLog << "    Swipe detect" << std::endl << std::endl;
                    break;
                }
            }
        }
        mLastFrame = controller.frame();

        if (translateProb > 0.5 && isSwipe)
        //if (isSwipe)
        {
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->move(Ogre::Vector3(translate.x, translate.y, translate.z) * (-0.01));
        }
        if (rotateProb > 0.5)
        {
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->rotate(Ogre::Vector3(rotateAxis.x, rotateAxis.y, rotateAxis.z) * (-1), Ogre::Radian(rotateAngle), Ogre::Node::TS_PARENT);
        }
        if (scaleProb > 0.5)
        {
        //    MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->scale(scale, scale, scale);
        }
    }

    void LeapMotionApp::onFocusGained(const Leap::Controller& controller)
    {
        MagicLog << "LeapMotionApp::onFocusGained" << std::endl;
    }

    void LeapMotionApp::onFocusLost(const Leap::Controller& controller)
    {
        MagicLog << "LeapMotionApp::onFocusLost" << std::endl;
    }

    void LeapMotionApp::OpenMesh3D()
    {
        std::string fileName;
        if (MagicCore::ToolKit::GetSingleton()->FileOpenDlg(fileName))
        {
            MagicDGP::Mesh3D* pMesh = MagicDGP::Parser::ParseMesh3D(fileName);
            if (pMesh != NULL)
            {
                pMesh->UpdateNormal();
                pMesh->UnifyPosition(2.0);
                if (mpMesh != NULL)
                {
                    delete mpMesh;
                }
                mpMesh = pMesh;
                MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderMesh", "MyCookTorrance", mpMesh);
            }
        }
    }

    void LeapMotionApp::SetupScene(void)
    {
        MagicLog << "LeapMotionApp::SetupScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light*  sl = pSceneMgr->createLight("SimpleLight");
        sl->setPosition(10, 10, 20);
        sl->setDiffuseColour(0.8, 0.8, 0.8);
        sl->setSpecularColour(0.5, 0.5, 0.5);
    }

    void LeapMotionApp::ShutdownScene(void)
    {
        MagicLog << "LeapMotionApp::ShutdownScene" << std::endl;
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue::Black);
        pSceneMgr->destroyLight("SimpleLight");
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("RenderMesh");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetOrientation();
    }
}
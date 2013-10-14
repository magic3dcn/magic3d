#include "LeapMotionApp.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Parser.h"
#include "../Common/RenderSystem.h"

namespace MagicApp
{
    LeapMotionData::LeapMotionData() :
        mFrameIndex(0)
    {

    }
        
    LeapMotionData::~LeapMotionData()
    {

    }

    void LeapMotionData::LoadData()
    {
        std::string fileName;
        if (MagicCore::ToolKit::GetSingleton()->FileOpenDlg(fileName))
        {
            std::ifstream fin(fileName);
            mTimeStamp.clear();
            mPalmPos.clear();
            mPalmVelocity.clear();
            char ch;
            int frameSize = 0;
            while(!fin.eof())
            {
                fin >> ch;
                int64_t time;
                MagicDGP::Vector3 palmPos, palmVelocity;
                fin >> time >> palmPos[0] >> palmPos[1] >> palmPos[2] >> palmVelocity[0] >> palmVelocity[1] >> palmVelocity[2];
                mTimeStamp.push_back(time);
                mPalmPos.push_back(palmPos);
                mPalmVelocity.push_back(palmVelocity);
                frameSize++;
            }
            fin.close();
            MagicLog << "LeapMotionData: " << frameSize << " frames" << std::endl;
        }
    }

    void LeapMotionData::GetCurrentData(MagicDGP::Vector3& palmPos, MagicDGP::Vector3& palmVelocity)
    {
        palmPos = mPalmPos.at(mFrameIndex);
        palmVelocity = mPalmVelocity.at(mFrameIndex);
    }

    int64_t LeapMotionData::GetCurrentTimeStamp()
    {
        return mTimeStamp.at(mFrameIndex);
    }

    int64_t LeapMotionData::GetNextTimeStamp()
    {
        return mTimeStamp.at( (mFrameIndex + 1) % mTimeStamp.size() );
    }

    void LeapMotionData::NextFrame()
    {
        mFrameIndex++;
        if (mFrameIndex == mTimeStamp.size())
        {
            mFrameIndex = 0;
        }
    }

    LeapMotionApp::LeapMotionApp() :
        mpMesh(NULL),
        mLeapMotinOn(false),
        mLeapLastTime(0),
        mIsLeapRecording(false),
        mIsLeapDataPlaying(false),
        mSysTimeAccum(0)
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
        if (mIsLeapDataPlaying)
        {
            mSysTimeAccum += timeElapsed;
            float currentTime = mLeapData.GetCurrentTimeStamp() / 1000000.f;
            MagicDGP::Vector3 palmPos, palmVelocity;
            mLeapData.GetCurrentData(palmPos, palmVelocity);
            while (true)
            {
                float nextTime = mLeapData.GetNextTimeStamp() / 1000000.f;
                if (nextTime < currentTime)
                {
                    mSysTimeAccum = 0;
                    mLeapData.NextFrame();
                    mLeapData.GetCurrentData(palmPos, palmVelocity);
                    break;
                }
                if ( (nextTime - currentTime) < mSysTimeAccum )
                {
                    mLeapData.NextFrame();
                    mLeapData.GetCurrentData(palmPos, palmVelocity);
                    mSysTimeAccum -= (nextTime - currentTime);
                    currentTime = nextTime;
                }
                else
                {
                    break;
                }
            }
            palmPos /= 500;
            //MagicLog << "Update: palmPos: " << palmPos[0] << " " << palmPos[1] << " " << palmPos[2] << std::endl;
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->setPosition(palmPos[0], palmPos[1], palmPos[2]);
        }

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
        if (arg.key == OIS::KC_Q)
        {
            if (mIsLeapRecording == false)
            {
                StartLeapRecorder();
            }
        }
        if (arg.key == OIS::KC_W)
        {
            if (mIsLeapRecording == true)
            {
                StopLeapRecorder();
            }
        }
        if (arg.key == OIS::KC_A)
        {
            PlayLeapData();
        }
        if (arg.key == OIS::KC_S)
        {
            StopLeapData();
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
        //const Leap::Frame frame = controller.frame();
        /*MagicLog << "Frame id: " << frame.id() << " hands: " << frame.hands().count() << " fingers: "
            << frame.fingers().count() << " tools: " << frame.tools().count() << " gestures: "
            << frame.gestures().count() << std::endl;*/
        //const Leap::Vector translate = frame.translation(mLastFrame);
        //float translateProb = frame.translationProbability(mLastFrame);
        //const Leap::Vector rotateAxis = frame.rotationAxis(mLastFrame);
        //float rotateAngle = frame.rotationAngle(mLastFrame);
        //float rotateProb = frame.rotationProbability(mLastFrame);
        //float scale = frame.scaleFactor(mLastFrame);
        //float scaleProb = frame.scaleProbability(mLastFrame);
        /*MagicLog << "  translate: " << translate.x << " " << translate.y << " " << translate.z << " " << translateProb << std::endl;
        MagicLog << "  rotate: " << rotateAxis.x << " " << rotateAxis.y << " " << rotateAxis.z << " " << rotateAngle << " " << rotateProb << std::endl;
        MagicLog << "  scale: " << scale << " " << scaleProb << std::endl;*/
        //MagicLog << std::endl;
        //bool isSwipe = false;
        //Leap::GestureList gestList = frame.gestures();
        //Leap::Vector swipeDir;
        //float swipeSpeed;
        //if (!gestList.isEmpty())
        //{
        //    for (int i = 0; i < gestList.count(); i++)
        //    {
        //        if (gestList[i].type() == Leap::Gesture::TYPE_SWIPE)
        //        {
        //            isSwipe = true;
        //            //Leap::SwipeGesture* swipeGes = dynamic_cast<Leap::SwipeGesture*>(&(gestList[i]));
        //            Leap::SwipeGesture swipeGes = gestList[i];
        //            swipeDir = swipeGes.direction(); 
        //            swipeSpeed = swipeGes.speed();
        //            MagicLog << "    Swipe detect: " << swipeSpeed << 
        //                " " << swipeDir.x << " " << swipeDir.y << " " << swipeDir.z << std::endl << std::endl;
        //            break;
        //        }
        //    }
        //}
        //Leap::HandList handList = frame.hands();
        //float minSpeed = 1500;
        //if (!handList.isEmpty())
        //{
        //    Leap::Vector palmVelocity;
        //    palmVelocity = handList[0].palmVelocity();
        //    if (palmVelocity.magnitude() > minSpeed)
        //    {
        //        MagicLog << "Time: " << frame.timestamp() << " Palm: " << palmVelocity.magnitude() << " "  << palmVelocity.x << " " 
        //            << palmVelocity.y << " " << palmVelocity.z << std::endl;
        //    }
        //    if (palmVelocity.magnitude() > minSpeed)
        //    {
        //        int64_t currentTime = frame.timestamp();
        //        if ( (currentTime - mLeapLastTime) > 500000 ||
        //            (palmVelocity.x * mLeapLastPalmVelocity.x + palmVelocity.y * mLeapLastPalmVelocity.y) > 0 )
        //            //!(palmVelocity.x * mLeapLastPalmVelocity.x < 0 || palmVelocity.y * mLeapLastPalmVelocity.y < 0) )
        //        {

        //            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->yaw(Ogre::Degree(palmVelocity.x * 0.02), Ogre::Node::TS_PARENT);
        //            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->pitch(Ogre::Degree(palmVelocity.y * (-0.02)), Ogre::Node::TS_PARENT);
        //            mLeapLastPalmVelocity = palmVelocity;
        //            mLeapLastTime = currentTime;
        //        }
        //    }
        //}
        //mLastFrame = controller.frame();
        
        if (mIsLeapRecording)
        {
            const Leap::Frame frame = controller.frame();
            Leap::HandList handList = frame.hands();
            if (!handList.isEmpty())
            {
                Leap::Vector palmVelocity = handList[0].palmVelocity();
                Leap::Vector palmPosition = handList[0].palmPosition();
                int64_t currentTime = frame.timestamp();
                mLeapRecorder << "f " << currentTime << " " << palmPosition.x << " " << palmPosition.y << " " << palmPosition.z
                    << " " << palmVelocity.x << " " << palmVelocity.y << " " << palmVelocity.z << std::endl;
            }
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
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->resetToInitialState();
    }

    void LeapMotionApp::StartLeapRecorder()
    {
        std::string fileName;
        if (MagicCore::ToolKit::GetSingleton()->FileSaveDlg(fileName))
        {
            mLeapRecorder.open(fileName);
            mIsLeapRecording = true;
        }
    }

    void LeapMotionApp::StopLeapRecorder()
    {
        mLeapRecorder.close();
        mIsLeapRecording = false;
    }

    void LeapMotionApp::PlayLeapData()
    {
        mLeapData.LoadData();
        mIsLeapDataPlaying = true;
        MagicDGP::Mesh3D* pMesh = MagicDGP::Parser::ParseMesh3D("../../Media/Model/ball.obj");
        pMesh->UnifyPosition(0.1);
        pMesh->UpdateNormal();
        MagicCore::RenderSystem::GetSingleton()->RenderMesh3D("RenderMesh", "MyCookTorrance", pMesh);
        delete pMesh;
        mSysTimeAccum = 0;
    }

    void LeapMotionApp::StopLeapData()
    {
        mIsLeapDataPlaying = false;
    }
}
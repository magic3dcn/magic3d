#include "stdafx.h"
#include "ReconstructionApp.h"
#include "Tool/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "../Common/ToolKit.h"
#include "../DGP/Registration.h"
#include "../DGP/SignedDistanceFunction.h"
#include "../DGP/Parser.h"
#include "../DGP/MeshReconstruction.h"
#include "../DGP/Sampling.h"
#include "../Common/AppManager.h"
#include "PointShopApp.h"
//#include "../Common/MagicOgre.h"

namespace MagicApp
{
    ReconstructionApp::ReconstructionApp() :
        mUsingViewTool(false),
        mIsScannerDisplaying(false),
        mLeftLimit(-1000.f), 
        mRightLimit(1000.f), 
        mTopLimit(1000.f), 
        mDownLimit(-1000.f), 
        mFrontLimit(-200.f), 
        mBackLimit(-2200.f),
        mFrameStartIndex(0),
        mFrameEndIndex(0),
        mFrameCurrent(0),
        mIsNeedRangeLimitCaculation(false)
    {
    }

    ReconstructionApp::~ReconstructionApp()
    {
    }

    bool ReconstructionApp::Enter(void)
    {
        InfoLog << "Enter ReconstructionApp" << std::endl;
        mUI.Setup();
        SetupRenderScene();
        return SetupDevice();
    }

    bool ReconstructionApp::Update(float timeElapsed)
    {
        if (mIsScannerDisplaying)
        {
            UpdateScannerDisplay();
        }
        return true;
    }

    bool ReconstructionApp::Exit(void)
    {
        ReleaseRenderScene();
        ReleaseDevice();
        mUI.Shutdown();
        InfoLog << "ReconstructionApp::Exit" << std::endl;
        return true;
    }

    bool ReconstructionApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        if (mUsingViewTool)
        {
            MagicDGP::ViewTool::MouseMode mm;
            if (arg.state.buttonDown(OIS::MB_Left))
            {
                mm = MagicDGP::ViewTool::MM_Left_Down;
            }
            else if (arg.state.buttonDown(OIS::MB_Middle))
            {
                mm = MagicDGP::ViewTool::MM_Middle_Down;
            }
            else if (arg.state.buttonDown(OIS::MB_Right))
            {
                mm = MagicDGP::ViewTool::MM_Right_Down;
            }
            else
            {
                mm = MagicDGP::ViewTool::MM_None;
            }
            mViewTool.MouseMoved(arg.state.X.abs, arg.state.Y.abs, mm);
        }
        
        return true;
    }

    bool ReconstructionApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mUsingViewTool)
        {
            mViewTool.MousePressed(arg.state.X.abs, arg.state.Y.abs);
        }
        
        return true;
    }

    void ReconstructionApp::SetupRenderScene(void)
    {
        MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->setPosition(0, 0, 500);
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light* frontLight = pSceneMgr->createLight("frontLight");
        frontLight->setPosition(0, 0, 100000);
        frontLight->setDiffuseColour(0.8, 0.8, 0.8);
        frontLight->setSpecularColour(0.5, 0.5, 0.5);
    }

    void ReconstructionApp::ReleaseRenderScene(void)
    {
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->destroyLight("frontLight");
        MagicCore::RenderSystem::GetSingleton()->SetupCameraDefaultParameter();
        MagicCore::RenderSystem::GetSingleton()->HideRenderingObject("ScannerDepth");
        if (MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasSceneNode("ModelNode"))
        {
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->resetToInitialState();
        } 
        mUsingViewTool = false;
    }

    bool ReconstructionApp::SetupDevice()
    {
        if (MagicCore::ToolKit::GetSingleton()->IsONIInitialized() == false)
        {
            openni::Status rc = openni::OpenNI::initialize();
            if (rc != openni::STATUS_OK)
            {
                InfoLog << "OpenNI initialize failed: " << openni::OpenNI::getExtendedError() << std::endl;
                return false;
            }
            else
            {
                InfoLog << "OpenNI initialize succeed" << std::endl;
                MagicCore::ToolKit::GetSingleton()->SetONIInitialized(true);
            }
        }
        mLeftLimit = -1000.f; 
        mRightLimit = 1000.f;
        mTopLimit = 1000.f; 
        mDownLimit = -1000.f; 
        mFrontLimit = -200.f;
        mBackLimit = -2200.f;

        return true;
    }

    void ReconstructionApp::ReleaseDevice()
    {
         if (mIsScannerDisplaying)
        {
            mDevice.close();
            mDepthStream.destroy();
            mIsScannerDisplaying = false;
        }
    }

    void ReconstructionApp::CoarseRangeLimitCalculation(const std::vector<MagicMath::Vector3>& posList)
    {
        mLeftLimit = mDownLimit = 5000.f;
        mRightLimit = mTopLimit = -5000.f;
        mFrontLimit = -5000.f;
        mBackLimit = -500.f;
        int posNum = posList.size();
        for (int i = 0; i < posNum; i++)
        {
            MagicMath::Vector3 pos = posList.at(i);
            if (pos[0] < mLeftLimit)
            {
                mLeftLimit = pos[0];
            }
            if (pos[0] > mRightLimit)
            {
                mRightLimit = pos[0];
            }
            if (pos[1] < mDownLimit)
            {
                mDownLimit = pos[1];
            }
            if (pos[1] > mTopLimit)
            {
                mTopLimit = pos[1];
            }
            if (pos[2] > -(1.0e-15))
            {
                continue;
            }
            if (pos[2] > mFrontLimit)
            {
                mFrontLimit = pos[2];
            }
            if (pos[2] < mBackLimit)
            {
                mBackLimit = pos[2];
            }
        }
        DebugLog << "Coarse Limit: " << mLeftLimit << " " << mRightLimit << " " << mDownLimit << " " << mTopLimit << 
            " " << mFrontLimit << " " << mBackLimit << std::endl;
    }

    void ReconstructionApp::UpdateScannerDisplay()
    {
        openni::PlaybackControl* pPC = mDevice.getPlaybackControl();
        pPC->seek(mDepthStream, mFrameCurrent);
        mFrameCurrent++;
        if (mFrameCurrent > mFrameEndIndex)
        {
            mFrameCurrent = mFrameStartIndex;
        }

        openni::VideoFrameRef depthFrame;
        int changeIndex;
        openni::VideoStream** pStreams = new openni::VideoStream*[1];
        pStreams[0] = &mDepthStream;
        openni::OpenNI::waitForAnyStream(pStreams, 1, &changeIndex);
        if (changeIndex == 0)
        {
            mDepthStream.readFrame(&depthFrame);
        }
        delete []pStreams;

        if (depthFrame.isValid())
        {
            const openni::DepthPixel* pDepth = (const openni::DepthPixel*)depthFrame.getData();
            int resolutionX = depthFrame.getVideoMode().getResolutionX();
            int resolutionY = depthFrame.getVideoMode().getResolutionY();
            std::vector<MagicMath::Vector3> posList;
            for(int y = 0; y < resolutionY; y++)  
            {  
                for(int x = 0; x < resolutionX; x++)  
                {
                    openni::DepthPixel depth = pDepth[y * resolutionX + x]; 
                    float rx, ry, rz;
                    openni::CoordinateConverter::convertDepthToWorld(mDepthStream, 
                        x, y, depth, &rx, &ry, &rz);
                    MagicMath::Vector3 pos(-rx, ry, -rz);
                    posList.push_back(pos);
                }
            }
            if (mIsNeedRangeLimitCaculation)
            {
                CoarseRangeLimitCalculation(posList);
                mIsNeedRangeLimitCaculation = false;
            }
            std::vector<MagicMath::Vector3> norList;
            for (int y = 0; y < resolutionY; y++)
            {
                for (int x = 0; x < resolutionX; x++)
                {
                    if ((y == 0) || (y == resolutionY - 1) || (x == 0) || (x == resolutionX - 1))
                    {
                        norList.push_back(MagicMath::Vector3(0, 0, 1));
                        continue;
                    }
                    MagicMath::Vector3 pos = posList.at(y * resolutionX + x);
                    if (pos[0] < mLeftLimit || pos[0] > mRightLimit ||
                        pos[1] < mDownLimit || pos[1] > mTopLimit ||
                        pos[2] > mFrontLimit || pos[2] < mBackLimit)
                    {
                        norList.push_back(MagicMath::Vector3(0, 0, 1));
                        continue;
                    }
                    MagicMath::Vector3 dirX = posList.at(y * resolutionX + x + 1) - posList.at(y * resolutionX + x - 1);
                    MagicMath::Vector3 dirY = posList.at((y + 1) * resolutionX + x) - posList.at((y - 1) * resolutionX + x);
                    MagicMath::Vector3 nor = dirX.CrossProduct(dirY);
                    double len = nor.Normalise();
                    if (len > 1.0e-15)
                    {
                        norList.push_back(nor);
                    }
                    else
                    {
                        norList.push_back(MagicMath::Vector3(0, 0, 1));
                    }
                }
            }
            //Rendering Point Set
            Ogre::ManualObject* pMObj = NULL;
            Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
            char psName[20] = "ScannerDepth";
            if (pSceneMgr->hasManualObject(psName))
            {
                pMObj = pSceneMgr->getManualObject(psName);
                pMObj->clear();
            }
            else
            {
                pMObj = pSceneMgr->createManualObject(psName);
                pSceneMgr->getRootSceneNode()->attachObject(pMObj);
            }
            pMObj->begin("MyCookTorrancePoint", Ogre::RenderOperation::OT_POINT_LIST);
            int pointNum = posList.size();
            for (int i = 0; i < pointNum; i++)
            {
                MagicMath::Vector3 pos = posList.at(i);
                if (pos[0] < mLeftLimit || pos[0] > mRightLimit ||
                    pos[1] < mDownLimit || pos[1] > mTopLimit ||
                    pos[2] > mFrontLimit || pos[2] < mBackLimit)
                {
                    continue;
                }
                MagicMath::Vector3 nor = norList.at(i);
                pMObj->position(pos[0], pos[1], pos[2]);
                pMObj->normal(nor[0], nor[1], nor[2]);
                pMObj->colour(0.86, 0.86, 0.86);
            }
            pMObj->end();
        }
    }

    bool ReconstructionApp::OpenSceneRecord(int& frameNum)
    {
        std::string fileName;
        char filterName[] = "ONI Files(*.oni)\0*.oni\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            if (mIsScannerDisplaying)
            {
                mDevice.close();
                mDepthStream.destroy();
            }
            openni::Status rc = mDevice.open(fileName.c_str());
            if (rc != openni::STATUS_OK)
            {
                DebugLog << "ReconstructionApp::OpenSceneRecord failed: " << openni::OpenNI::getExtendedError() << std::endl;
                return false;
            }
            rc = mDepthStream.create(mDevice, openni::SENSOR_DEPTH);
            if (rc != openni::STATUS_OK)
            {
                DebugLog << "DepthStream create failed: " << openni::OpenNI::getExtendedError() << std::endl;
                return false;
            }
            mDepthStream.start();
            mIsScannerDisplaying = true;
            //mIsNeedRangeLimitCaculation = true;
            openni::PlaybackControl* pPC = mDevice.getPlaybackControl();
            mFrameStartIndex = 0;
            mFrameEndIndex = pPC->getNumberOfFrames(mDepthStream);
            mFrameCurrent = 0;
            frameNum = mFrameEndIndex;
            return true;
        }
        else
        {
            return false;
        }
    }

    void ReconstructionApp::SetTimeStart(int frameIndex)
    {
        if (frameIndex <= mFrameEndIndex)
        {
            mFrameStartIndex = frameIndex;
            mFrameCurrent = frameIndex;
        }
    }

    void ReconstructionApp::SetTimeEnd(int frameIndex)
    {
        if (frameIndex >= mFrameStartIndex)
        {
            mFrameEndIndex = frameIndex;
            mFrameCurrent = frameIndex;
        }
    }

    void ReconstructionApp::ChangeLeftRange(int rel)
    {
        float temp = mLeftLimit + rel / 5;
        if (temp > -5000.f && temp < mRightLimit)
        {
            mLeftLimit = temp;
        }
    }

    void ReconstructionApp::ChangeRightRange(int rel)
    {
        float temp = mRightLimit + rel / 5;
        if (temp < 5000.f && temp > mLeftLimit)
        {
            mRightLimit = temp;
        }
    }

    void ReconstructionApp::ChangeTopRange(int rel)
    {
        float temp = mTopLimit + rel / 5;
        if (temp < 5000.f && temp > mDownLimit)
        {
            mTopLimit = temp;
        }
    }

    void ReconstructionApp::ChangeDownRange(int rel)
    {
        float temp = mDownLimit + rel / 5;
        if (temp > -5000.f && temp < mTopLimit)
        {
            mDownLimit = temp;
        }
    }

    void ReconstructionApp::ChangeFrontRange(int rel)
    {
        float temp = mFrontLimit - rel / 5;
        if (temp < -300.f && temp > mBackLimit)
        {
            mFrontLimit = temp;
        }
    }

    void ReconstructionApp::ChangeBackRange(int rel)
    {
        float temp = mBackLimit - rel / 5;
        if (temp > -5000.f && temp < mFrontLimit)
        {
            mBackLimit = temp;
        }
    }

    void ReconstructionApp::ExportPointSet(void)
    {
        std::string path = "../../Media/FaceData/PointSet/f";
        for (int frameId = mFrameStartIndex; frameId < mFrameEndIndex; frameId++)
        {
            std::stringstream ss;
            ss << path << frameId << ".obj";
            std::string fileName;
            ss >> fileName;
            MagicDGP::Point3DSet* pPointSet = GetPointSetFromRecord(frameId);
            MagicDGP::Parser::ExportPointSet(fileName, pPointSet);
            delete pPointSet;
        }
    }

    void ReconstructionApp::PointSetRegistration()
    {
        DebugLog << "Coarse Limit: " << mLeftLimit << " " << mRightLimit << " " << mDownLimit << " " << mTopLimit << 
             " " << mFrontLimit << " " << mBackLimit << std::endl;
        mIsScannerDisplaying = false;
        //
        // Do Registration
        //
        //initialize
        mUI.SetProgressBarRange(mFrameEndIndex - mFrameStartIndex);
        DebugLog << "Create SignedDistanceFunction" << std::endl;
        MagicDGP::SignedDistanceFunction sdf(400, 400, 400, mLeftLimit, mRightLimit, mDownLimit, mTopLimit, mBackLimit, mFrontLimit);
        DebugLog << "Create SignedDistanceFunction Finish" << std::endl;
        MagicMath::HomoMatrix4 lastTrans;
        lastTrans.Unit();
        MagicDGP::Point3DSet* pPointSet = GetPointSetFromRecord(mFrameStartIndex);
        sdf.UpdateFineSDF(pPointSet, &lastTrans);
        delete pPointSet;
        pPointSet = sdf.ExtractFinePointCloud();
        MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("ScannerDepth", "MyCookTorrancePoint", pPointSet);
        MagicCore::RenderSystem::GetSingleton()->Update();
        for (int frameIndex = mFrameStartIndex + 1; frameIndex <= mFrameEndIndex; frameIndex++)
        {
            float timeStart = MagicCore::ToolKit::GetTime();
            mUI.SetProgressBarPosition(frameIndex - mFrameStartIndex);
            DebugLog << "Fusion Point Set: " << frameIndex << " -------------------------------"<< std::endl;
            MagicDGP::Point3DSet* pNewPC = GetPointSetFromRecord(frameIndex);//
            MagicMath::HomoMatrix4 newTrans;//
            DebugLog << "    Get " << pNewPC->GetPointNumber() << " PointSetFromRecord: " << MagicCore::ToolKit::GetTime() - timeStart << std::endl;
            float timeRegistrate = MagicCore::ToolKit::GetTime();
            MagicDGP::Registration registrate;
            registrate.ICPRegistrateEnhance(pPointSet, pNewPC, &lastTrans, &newTrans, mDepthStream);//
            DebugLog << "    Fusion: ICP Registration: " << MagicCore::ToolKit::GetTime() - timeRegistrate << std::endl;
            float timeUpdateSDF = MagicCore::ToolKit::GetTime();
            MagicMath::HomoMatrix4 newTransInv = newTrans.Inverse();//
            sdf.UpdateSDF(pNewPC, &newTransInv);//
            DebugLog << "    Fusion: Update SDF: " << MagicCore::ToolKit::GetTime() - timeUpdateSDF << std::endl;
            lastTrans = newTrans;
            delete pPointSet;
            delete pNewPC;
            pNewPC = NULL;
            float timeExtract = MagicCore::ToolKit::GetTime();
            pPointSet = sdf.ExtractFinePointCloud();//
            DebugLog << "    Fusion: Extract Point Set: " << MagicCore::ToolKit::GetTime() - timeExtract << std::endl;
            MagicCore::RenderSystem::GetSingleton()->RenderPoint3DSet("ScannerDepth", "MyCookTorrancePoint", pPointSet);
            MagicCore::RenderSystem::GetSingleton()->Update();
            DebugLog << "One iteration time: " << MagicCore::ToolKit::GetTime() - timeStart << std::endl;
        }
        //
        //mUI.StartPostProcess();
        pPointSet->SetHasNormal(true);
        MagicCore::AppManager::GetSingleton()->EnterApp(new PointShopApp, "PointShopApp");
        PointShopApp* pPSA = dynamic_cast<PointShopApp* >(MagicCore::AppManager::GetSingleton()->GetApp("PointShopApp"));
        if (pPSA != NULL)
        {
            pPSA->SetupFromPointsetInput(pPointSet);
        }
        else
        {
            WarnLog << "Get PointShopApp Failed" << std::endl;
        }
        pPointSet = NULL;// pPointSet has been transfered to PointShopApp
        DebugLog << "ReconstructionApp::PointSetRegistrationEnhance: End" << std::endl;
    }

    MagicDGP::Point3DSet* ReconstructionApp::GetPointSetFromRecord(int frameId)
    {
        DebugLog << "ReconstructionApp::GetPointSetFromRecord " << frameId << std::endl;
        openni::PlaybackControl* pPC = mDevice.getPlaybackControl();
        pPC->seek(mDepthStream, frameId);
        openni::VideoFrameRef depthFrame;
        int changeIndex;
        openni::VideoStream** pStreams = new openni::VideoStream*[1];
        pStreams[0] = &mDepthStream;
        openni::OpenNI::waitForAnyStream(pStreams, 1, &changeIndex);
        if (changeIndex == 0)
        {
            mDepthStream.readFrame(&depthFrame);
        }
        delete []pStreams;

        if (depthFrame.isValid())
        {
            const openni::DepthPixel* pDepth = (const openni::DepthPixel*)depthFrame.getData();
            int resolutionX = depthFrame.getVideoMode().getResolutionX();
            int resolutionY = depthFrame.getVideoMode().getResolutionY();
            std::vector<MagicMath::Vector3> posList;
            for(int y = 0; y < resolutionY; y++)  
            {  
                for(int x = 0; x < resolutionX; x++)  
                {
                    openni::DepthPixel depth = pDepth[y * resolutionX + x]; 
                    float rx, ry, rz;
                    openni::CoordinateConverter::convertDepthToWorld(mDepthStream, 
                        x, y, depth, &rx, &ry, &rz);
                    MagicMath::Vector3 pos(-rx, ry, -rz);
                    posList.push_back(pos);
                }
            }
            //Smooth
            std::vector<MagicMath::Vector3> smoothPosList;
            float depthThre = 100;
            for (int y = 0; y < resolutionY; y++)
            {
                for (int x = 0; x < resolutionX ; x++)
                {
                    MagicMath::Vector3 pos = posList.at(y * resolutionX + x);
                    if ((y == 0) || (y == resolutionY - 1) || (x == 0) || (x == resolutionX - 1))
                    {
                        smoothPosList.push_back(pos);
                        continue;
                    }
                    if (pos[0] < mLeftLimit || pos[0] > mRightLimit ||
                        pos[1] < mDownLimit || pos[1] > mTopLimit ||
                        pos[2] > mFrontLimit || pos[2] < mBackLimit)
                    {
                        smoothPosList.push_back(pos);
                        continue;
                    }
                    if (pos.Length() < 1.0e-15)
                    {
                        smoothPosList.push_back(pos);
                        continue;
                    }
                    std::vector<MagicMath::Vector3> neighbotPos;
                    MagicMath::Vector3 pos1 = posList.at(y * resolutionX + x - 1);
                    if (pos1.Length() > 1.0e-15)
                    {
                        if (fabs(pos[2] - pos1[2]) < depthThre)
                        {
                            neighbotPos.push_back(pos1);
                        }
                        else
                        {
                            smoothPosList.push_back(MagicMath::Vector3(0, 0, 0));
                            continue;
                        }
                    }
                    MagicMath::Vector3 pos2 = posList.at(y * resolutionX + x + 1);
                    if (pos2.Length() > 1.0e-15)
                    {
                        if (fabs(pos[2] - pos2[2]) < depthThre)
                        {
                            neighbotPos.push_back(pos2);
                        }
                        else
                        {
                            smoothPosList.push_back(MagicMath::Vector3(0, 0, 0));
                            continue;
                        }
                    }
                    MagicMath::Vector3 pos3 = posList.at((y - 1) * resolutionX + x);
                    if (pos3.Length() > 1.0e-15)
                    {
                        if (fabs(pos[2] - pos3[2]) < depthThre)
                        {
                            neighbotPos.push_back(pos3);
                        }
                        else
                        {
                            smoothPosList.push_back(MagicMath::Vector3(0, 0, 0));
                            continue;
                        }
                    }
                    MagicMath::Vector3 pos4 = posList.at((y + 1) * resolutionX + x);
                    if (pos4.Length() > 1.0e-15)
                    {
                        if (fabs(pos[2] - pos4[2]) < depthThre)
                        {
                            neighbotPos.push_back(pos4);
                        }
                        else
                        {
                            smoothPosList.push_back(MagicMath::Vector3(0, 0, 0));
                            continue;
                        }
                    }
                    if (neighbotPos.size() == 0)
                    {
                        smoothPosList.push_back(pos);
                    }
                    else
                    {
                        MagicMath::Vector3 avgPos(0, 0, 0);
                        for (int i = 0; i < neighbotPos.size(); i++)
                        {
                            avgPos += neighbotPos.at(i);
                        }
                        avgPos /= neighbotPos.size();
                        smoothPosList.push_back( (pos + avgPos) / 2);
                    }
                }
            }
            posList = smoothPosList;
            //
            std::vector<MagicMath::Vector3> norList;
            for (int y = 0; y < resolutionY; y++)
            {
                for (int x = 0; x < resolutionX; x++)
                {
                    if ((y == 0) || (y == resolutionY - 1) || (x == 0) || (x == resolutionX - 1))
                    {
                        norList.push_back(MagicMath::Vector3(0, 0, 1));
                        continue;
                    }
                    MagicMath::Vector3 pos = posList.at(y * resolutionX + x);
                    if (pos[0] < mLeftLimit || pos[0] > mRightLimit ||
                        pos[1] < mDownLimit || pos[1] > mTopLimit ||
                        pos[2] > mFrontLimit || pos[2] < mBackLimit)
                    {
                        norList.push_back(MagicMath::Vector3(0, 0, 1));
                        continue;
                    }
                    MagicMath::Vector3 dirX = posList.at(y * resolutionX + x + 1) - posList.at(y * resolutionX + x - 1);
                    MagicMath::Vector3 dirY = posList.at((y + 1) * resolutionX + x) - posList.at((y - 1) * resolutionX + x);
                    MagicMath::Vector3 nor = dirX.CrossProduct(dirY);
                    double len = nor.Normalise();
                    if (len > 1.0e-15)
                    {
                        norList.push_back(nor);
                    }
                    else
                    {
                        norList.push_back(MagicMath::Vector3(0, 0, 1));
                    }
                }
            }
            MagicDGP::Point3DSet* pPS = new MagicDGP::Point3DSet;
            int pointNum = posList.size();
            for (int i = 0; i < pointNum; i++)
            {
                MagicMath::Vector3 pos = posList.at(i);
                if (pos[0] < mLeftLimit || pos[0] > mRightLimit ||
                    pos[1] < mDownLimit || pos[1] > mTopLimit ||
                    pos[2] > mFrontLimit || pos[2] < mBackLimit)
                {
                    continue;
                }
                MagicDGP::Point3D* pNewPoint = new MagicDGP::Point3D(pos, norList.at(i));
                pPS->InsertPoint(pNewPoint);
            }
            return pPS;
        }
        else
        {
            return NULL;
        }
    }

}
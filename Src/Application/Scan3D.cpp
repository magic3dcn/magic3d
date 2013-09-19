////#include "StdAfx.h"
#include "Scan3D.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "OpenNI.h"
#include <math.h>
#include <fstream>
#include "../DGP/Vector3.h"

namespace MagicApp
{
    Scan3D::Scan3D() :
        mHasInitilised(false),
        mIsScanning(false),
        mIsRecording(false),
        mIsRecordScanning(false),
        mpCanvasColor(NULL),
        mpCanvasDepth(NULL)
    {
    }

    Scan3D::~Scan3D()
    {
        ClearApp();
    }

    bool Scan3D::Enter()
    {
        MagicLog << "Enter Scan3D" << std::endl;
        Init();
        mUI.Setup();
        SetRenderEnvironment();
        ResumeAppState();
        return true;
    }

    bool Scan3D::Update(float timeElapsed)
    {
        if (mIsScanning || mIsRecordScanning)
        {
            UpdateScanner();
        }
        return true;
    }

    bool Scan3D::Exit()
    {
        PauseAppState();
        mUI.Shutdown();
        DestroyRenderEnvironment();
        return true;
    }

    void Scan3D::PlayScanner()
    {
        if (mIsScanning == false)
        {
            if (mIsRecordScanning)
            {
                mMediaStream.StopRecordScanner();
                mIsRecordScanning = false;
            }
            mMediaStream.StartScanner();
            mIsScanning = true;

        }
        else
        {
            mMediaStream.StopScanner();
            mIsScanning = false; 
        }
    }

    void Scan3D::Record()
    {
        if (mIsRecording == false)
        {
            mMediaStream.StartRecorder();
            mIsRecording = true;
        }
        else
        {
            mMediaStream.StopRecorder();
            mIsRecording = false;
        }
    }

    void Scan3D::PlayRecordScanner()
    {
        if (mIsRecordScanning == false)
        {
            if (mIsScanning)
            {
                mMediaStream.StopScanner();
                mIsScanning = false;
            }
            mMediaStream.StartRecordScanner();
            mIsRecordScanning = true;
        }
        else
        {
            mMediaStream.StopRecordScanner();
            mIsRecordScanning = false;
        }
    }

    void Scan3D::Init()
    {
        if (mHasInitilised == false)
        {
            mMediaStream.Init();
            InitCanvas(640, 480);
            mHasInitilised = true;
        }
    }

    void Scan3D::InitCanvas(int resolutionX, int resolutionY)
    {
        float w = 0.45f;
        float h = w * resolutionY * 1024.f / resolutionX / 768.f;
        float leftCX = -0.5f;
        float leftCY = 0.1f;
        float rightCX = 0.5f;
        float rightCY = 0.1f;
        mpTexColor = Ogre::TextureManager::getSingleton().createManual(  
                        "Tex_Canvas_Color",
                        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,  
                        Ogre::TEX_TYPE_2D,
                        resolutionX,
                        resolutionY,
                        0,                   // number of mipmaps   
                        Ogre::PF_B8G8R8A8,     // pixel format   
                        Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE // usage, for textures updated very often   
                        ); 
        Ogre::HardwarePixelBufferSharedPtr pPixelBufferColor = mpTexColor->getBuffer();
        // Lock the pixel buffer and get a pixel box   
        unsigned char* pColorBuffer = static_cast<unsigned char*>(pPixelBufferColor->lock(0, resolutionX * resolutionY * 4, Ogre::HardwareBuffer::HBL_DISCARD));
        for(int y = 0; y < resolutionY; y++)  
        {  
            for(int x = 0; x < resolutionX; x++)  
            {  
                pColorBuffer[((y * resolutionX) + x) * 4 + 0 ] = unsigned char(108); // B   
                pColorBuffer[((y * resolutionX) + x) * 4 + 1 ] = unsigned char(97);  // G   
                pColorBuffer[((y * resolutionX) + x) * 4 + 2 ] = unsigned char(235); // R   
                pColorBuffer[((y * resolutionX) + x) * 4 + 3 ] = 1;
            }
        }  
        // Unlock the pixel buffer   
        pPixelBufferColor->unlock();
        Ogre::MaterialPtr pMatColor = Ogre::MaterialManager::getSingleton().create(  
            "Mat_Canvas_Color",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        pMatColor->getTechnique(0)->getPass(0)->createTextureUnitState("Tex_Canvas_Color"); 
        pMatColor->getTechnique(0)->getPass(0)->setDiffuse(0.8, 0.8, 0.8, 1);
        pMatColor->getTechnique(0)->getPass(0)->setAmbient(0.4, 0.4, 0.4);
        mpCanvasColor = new Ogre::Rectangle2D(true);
        mpCanvasColor->setCorners(leftCX - w, leftCY + h, leftCX + w, leftCY - h);
      //  mpCanvasColor->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
        mpCanvasColor->setMaterial("Mat_Canvas_Color");

        mpTexDepth = Ogre::TextureManager::getSingleton().createManual(  
                        "Tex_Canvas_Depth",
                        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,  
                        Ogre::TEX_TYPE_2D,
                        resolutionX,
                        resolutionY,
                        0,                   // number of mipmaps   
                        Ogre::PF_L16,         // pixel format   
                        Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE // usage, for textures updated very often   
                        ); 
        Ogre::HardwarePixelBufferSharedPtr pPixelBufferDepth = mpTexDepth->getBuffer();
        // Lock the pixel buffer and get a pixel box   
        unsigned short* pDepthBuffer = static_cast<unsigned short*>(pPixelBufferDepth->lock(0, resolutionX * resolutionY * 2, Ogre::HardwareBuffer::HBL_DISCARD));
        for(int y = 0; y < resolutionY; y++)  
        {  
            for(int x = 0; x < resolutionX; x++)  
            {  
                pDepthBuffer[((y * resolutionX) + x) * 1 + 0 ] = unsigned short(32768);
            }
        }  
        // Unlock the pixel buffer   
        pPixelBufferDepth->unlock();
        Ogre::MaterialPtr pMatDepth = Ogre::MaterialManager::getSingleton().create(  
            "Mat_Canvas_Depth",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        pMatDepth->getTechnique(0)->getPass(0)->createTextureUnitState("Tex_Canvas_Depth"); 
        pMatDepth->getTechnique(0)->getPass(0)->setDiffuse(0.8, 0.8, 0.8, 1);
        pMatDepth->getTechnique(0)->getPass(0)->setAmbient(0.4, 0.4, 0.4);
        mpCanvasDepth = new Ogre::Rectangle2D(true);
        mpCanvasDepth->setCorners(rightCX - w, rightCY + h, rightCX + w, rightCY - h);
      //  mpCanvasDepth->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f * Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
        mpCanvasDepth->setMaterial("Mat_Canvas_Depth");
    }

    void Scan3D::ClearApp()
    {
        //Not done. Since UI has not done finally.
    }

    void Scan3D::SetRenderEnvironment()
    {
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light* frontLight = pSceneMgr->createLight("frontLight");
        frontLight->setPosition(0, 0, 10);
        //frontLight->setPosition(10, 10, 20);
        frontLight->setDiffuseColour(0.8, 0.8, 0.8);
        frontLight->setSpecularColour(0.5, 0.5, 0.5);
        if (mpCanvasColor != NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->attachObject(mpCanvasColor);
        }
        if (mpCanvasDepth != NULL)
        {
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->attachObject(mpCanvasDepth);
        }
    }

    void Scan3D::DestroyRenderEnvironment()
    {
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->destroyLight("frontLight");
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->detachObject(mpCanvasColor);
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->detachObject(mpCanvasDepth);
    }

    void Scan3D::UpdateScanner()
    {
        openni::VideoFrameRef colorFrame, depthFrame;
        mMediaStream.WaitStream(&colorFrame, &depthFrame);
        if (colorFrame.isValid())
        {
          //  MagicLog << "Update color frame: " << colorFrame.getWidth() << " " << colorFrame.getHeight() << std::endl;
            const openni::RGB888Pixel* pPixel = (const openni::RGB888Pixel*)colorFrame.getData();
            Ogre::HardwarePixelBufferSharedPtr pPixelBufferColor = mpTexColor->getBuffer();
            int resolutionX = mpTexColor->getWidth();
            int resolutionY = mpTexColor->getHeight();
            unsigned char* pColorBuffer = static_cast<unsigned char*>(pPixelBufferColor->lock(0, resolutionX * resolutionY * 4, Ogre::HardwareBuffer::HBL_DISCARD));
            for(int y = 0; y < resolutionY; y++)  
            {  
                for(int x = 0; x < resolutionX; x++)  
                {
                    openni::RGB888Pixel pixel = pPixel[y * resolutionX + x];
                    pColorBuffer[(y * resolutionX + x) * 4 + 0 ] = pixel.b;  // B   
                    pColorBuffer[(y * resolutionX + x) * 4 + 1 ] = pixel.g;  // G   
                    pColorBuffer[(y * resolutionX + x) * 4 + 2 ] = pixel.r;  // R   
                    pColorBuffer[(y * resolutionX + x) * 4 + 3 ] = 1;
                }
            }  
            // Unlock the pixel buffer   
            pPixelBufferColor->unlock();
        }
        if (depthFrame.isValid())
        {
          //  MagicLog << "Update depth frame: " << depthFrame.getWidth() << " " << depthFrame.getHeight() << std::endl;
            const openni::DepthPixel* pDepth = (const openni::DepthPixel*)depthFrame.getData();
            Ogre::HardwarePixelBufferSharedPtr pPixelBufferDepth = mpTexDepth->getBuffer();
            int resolutionX = mpTexDepth->getWidth();
            int resolutionY = mpTexDepth->getHeight();
            unsigned short* pDepthBuffer = static_cast<unsigned short*>(pPixelBufferDepth->lock(0, resolutionX * resolutionY * 2, Ogre::HardwareBuffer::HBL_DISCARD));
            for(int y = 0; y < resolutionY; y++)  
            {  
                for(int x = 0; x < resolutionX; x++)  
                {
                    openni::DepthPixel depth = pDepth[y * resolutionX + x]; 
                    pDepthBuffer[y * resolutionX + x] = depth << 5;
                }
            }  
            // Unlock the pixel buffer   
            pPixelBufferDepth->unlock();
        }
        //if (depthFrame.isValid())
        //{
        //    const openni::DepthPixel* pDepth = (const openni::DepthPixel*)depthFrame.getData();
        //    int resolutionX = depthFrame.getVideoMode().getResolutionX();
        //    int resolutionY = depthFrame.getVideoMode().getResolutionY();
        //    std::vector<MagicDGP::Vector3> posList;
        //    for(int y = 0; y < resolutionY; y++)  
        //    {  
        //        for(int x = 0; x < resolutionX; x++)  
        //        {
        //            openni::DepthPixel depth = pDepth[y * resolutionX + x]; 
        //            float rx, ry, rz;
        //            openni::CoordinateConverter::convertDepthToWorld(mMediaStream.GetDepthStream(), 
        //                x, y, depth, &rx, &ry, &rz);
        //            MagicDGP::Vector3 pos(rx / 500.f, ry / 500.f, -rz / 500.f);
        //            posList.push_back(pos);
        //        }
        //    }
        //    std::vector<MagicDGP::Vector3> norList;
        //    for (int y = 0; y < resolutionY; y++)
        //    {
        //        for (int x = 0; x < resolutionX; x++)
        //        {
        //            if ((y == 0) || (y == resolutionY - 1) || (x == 0) || (x == resolutionX - 1))
        //            {
        //                norList.push_back(MagicDGP::Vector3(0, 0, 1));
        //                continue;
        //            }
        //            if (posList.at(y * resolutionX + x)[2] > MagicDGP::Epsilon * (-1.f))
        //            {
        //                norList.push_back(MagicDGP::Vector3(0, 0, 1));
        //                continue;
        //            }
        //            MagicDGP::Vector3 dirX = posList.at(y * resolutionX + x + 1) - posList.at(y * resolutionX + x - 1);
        //            MagicDGP::Vector3 dirY = posList.at((y + 1) * resolutionX + x) - posList.at((y - 1) * resolutionX + x);
        //            MagicDGP::Vector3 nor = dirY.CrossProduct(dirX);
        //            MagicDGP::Real len = nor.Normalise();
        //            if (len > MagicDGP::Epsilon)
        //            {
        //                norList.push_back(nor);
        //            }
        //            else
        //            {
        //                norList.push_back(MagicDGP::Vector3(0, 0, 1));
        //            }
        //        }
        //    }
        //    //Rendering Point Set
        //    Ogre::ManualObject* pMObj = NULL;
        //    Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        //    char psName[20] = "ScannerDepth";
        //    if (pSceneMgr->hasManualObject(psName))
        //    {
        //        pMObj = pSceneMgr->getManualObject(psName);
        //        pMObj->clear();
        //    }
        //    else
        //    {
        //        pMObj = pSceneMgr->createManualObject(psName);
        //        pSceneMgr->getRootSceneNode()->attachObject(pMObj);
        //    }
        //    pMObj->begin("SimplePoint", Ogre::RenderOperation::OT_POINT_LIST);
        //    int pointNum = posList.size();
        //    for (int i = 0; i < pointNum; i++)
        //    {
        //        MagicDGP::Vector3 pos = posList.at(i);
        //        MagicDGP::Vector3 nor = norList.at(i);
        //        pMObj->position(pos[0], pos[1], pos[2]);
        //        pMObj->normal(nor[0], nor[1], nor[2]);
        //    }
        //    pMObj->end();
        //}
    }

    void Scan3D::CapturePointSet()
    {
        static int fileIndex = 0;
        openni::VideoFrameRef colorFrame, depthFrame;
        mMediaStream.WaitStream(&colorFrame, &depthFrame);
        if (depthFrame.isValid())
        {
            const openni::DepthPixel* pDepth = (const openni::DepthPixel*)depthFrame.getData();
            int resolutionX = mpTexDepth->getWidth();
            int resolutionY = mpTexDepth->getHeight();
            std::vector<MagicDGP::Vector3> posList;
            for(int y = 0; y < resolutionY; y++)  
            {  
                for(int x = 0; x < resolutionX; x++)  
                {
                    openni::DepthPixel depth = pDepth[y * resolutionX + x]; 
                    float rx, ry, rz;
                    openni::CoordinateConverter::convertDepthToWorld(mMediaStream.GetDepthStream(), 
                        x, y, depth, &rx, &ry, &rz);
                    MagicDGP::Vector3 pos(-rx, ry, rz);
                    posList.push_back(pos);
                }
            }
            char fileName[20];
            sprintf(fileName, "Scene_%d.obj", fileIndex);
            fileIndex++;
            std::ofstream fout(fileName);
            for (int j = 1; j < resolutionY - 1; j++)
            {
                for (int i = 1; i < resolutionX - 1; i++)
                {
                    MagicDGP::Vector3 dirX = posList.at(j * resolutionX + i + 1) - posList.at(j * resolutionX + i - 1);
                    MagicDGP::Vector3 dirY = posList.at((j + 1) * resolutionX + i) - posList.at((j - 1) * resolutionX + i);
                    MagicDGP::Vector3 nor = dirY.CrossProduct(dirX);
                    MagicDGP::Real len = nor.Normalise();
                    if (len > MagicDGP::Epsilon)
                    {
                        MagicDGP::Vector3 pos = posList.at(j * resolutionX + i);
                        fout << "vn " << nor[0] << " " << nor[1] << " " << nor[2] << std::endl;
                        fout << "v " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
                    }
                }
            }
            fout.close();
        }
    }

    void Scan3D::PauseAppState()
    {
        if (mIsScanning)
        {
            mMediaStream.StopScanner();
        }
        if (mIsRecording)
        {
            mMediaStream.StopRecorder();
        }
        if (mIsRecordScanning)
        {
            mMediaStream.StopRecordScanner();
        }
    }

    void Scan3D::ResumeAppState()
    {
        if (mIsScanning)
        {
            mMediaStream.StartScanner();
        }
        if (mIsRecording)
        {
            mMediaStream.StartRecorder();
        }
        if (mIsRecordScanning)
        {
            mMediaStream.StartRecordScanner();
        }
    }
}
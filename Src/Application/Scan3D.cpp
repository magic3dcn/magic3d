#include "StdAfx.h"
#include "Scan3D.h"
#include "../Common/LogSystem.h"
#include "../Common/RenderSystem.h"
#include "OpenNI.h"
#include <math.h>

namespace MagicApp
{
    Scan3D::Scan3D() :
        mHasInitilised(false),
        mHasScanStart(false),
        mpCanvasColor(NULL),
        mpCanvasDepth(NULL)
    {
    }

    Scan3D::~Scan3D()
    {
        DestroyCanvas();
    }

    bool Scan3D::Enter()
    {
        MagicLog << "Enter Scan3D" << std::endl;
        mUI.Setup();
        if (!mHasInitilised)
        {
            Init();
            mMediaStream.Init();
            mHasInitilised = true;
        }
        SetRenderEnvironment();
        return true;
    }

    bool Scan3D::Update(float timeElapsed)
    {
        if (mHasScanStart)
        {
            UpdateScanner();
        }
        return true;
    }

    bool Scan3D::Exit()
    {
        mUI.Shutdown();
        DestroyRenderEnvironment();
        return true;
    }

    MediaStream& Scan3D::GetMediaStream()
    {
        return mMediaStream;
    }

    bool Scan3D::IsScannerUpdate()
    {
        return mHasScanStart;
    }

    void Scan3D::StartUpdateScanner()
    {
        mHasScanStart = true;
    }

    void Scan3D::StopUpdateScanner()
    {
        mHasScanStart = false;
    }

    void Scan3D::Init()
    {
        //mMediaStream.Init();
        //InitCanvas(mMediaStream.GetResolutionX(), mMediaStream.GetResolutionY());
        InitCanvas(640, 480);
    }

    void Scan3D::InitCanvas(int resolutionX, int resolutionY)
    {
        float w = 0.4f;
        float h = w * resolutionY * 1024.f / resolutionX / 768.f;
        float leftCX = -0.45f;
        float leftCY = 0.1f;
        float rightCX = 0.45f;
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
           //     pDepthBuffer[((y * resolutionX) + x) * 2 + 1 ] = unsigned char(128);
           //     pDepthBuffer[((y * resolutionX) + x) * 4 + 2 ] = unsigned char(0);
           //     pDepthBuffer[((y * resolutionX) + x) * 4 + 3 ] = 1;
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

        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->attachObject(mpCanvasColor);
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getRootSceneNode()->attachObject(mpCanvasDepth);
    }

    void Scan3D::DestroyCanvas()
    {

    }

    void Scan3D::SetRenderEnvironment()
    {
        Ogre::SceneManager* pSceneMgr = MagicCore::RenderSystem::GetSingleton()->GetSceneManager();
        pSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
        Ogre::Light* frontLight = pSceneMgr->createLight("frontLight");
        frontLight->setPosition(0, 0, 10);
        frontLight->setDiffuseColour(0.8, 0.8, 0.8);
        frontLight->setSpecularColour(0.5, 0.5, 0.5);
    }

    void Scan3D::DestroyRenderEnvironment()
    {
        MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->destroyLight("frontLight");
    }

    void Scan3D::UpdateScanner()
    {
      //  static int colorNum = 0;
      //  static int depthNum = 0;
        //MagicLog << "Start Scan3D::UpdateScanner" << std::endl;
        openni::VideoFrameRef colorFrame, depthFrame;
        mMediaStream.WaitStream(&colorFrame, &depthFrame);
        if (colorFrame.isValid())
        {
           // MagicLog << "Update color frame: " << colorFrame.getWidth() << " " << colorFrame.getHeight() << std::endl;
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
       //     colorNum++;
        }
        if (depthFrame.isValid())
        {
           // MagicLog << "Update depth frame: " << depthFrame.getWidth() << " " << depthFrame.getHeight() << std::endl;
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
                    //pDepthBuffer[(y * resolutionX + x) * 4 + 0 ] = depth;  // B   
                    //pDepthBuffer[(y * resolutionX + x) * 4 + 1 ] = depth;  // G   
                    //pDepthBuffer[(y * resolutionX + x) * 4 + 2 ] = depth;  // R   
                    //pDepthBuffer[(y * resolutionX + x) * 4 + 3 ] = 1;
                    pDepthBuffer[y * resolutionX + x] = depth << 5;
                }
            }  
            // Unlock the pixel buffer   
            pPixelBufferDepth->unlock();
        //    depthNum++;
        }
       // MagicLog << "Update: color Num: " << colorNum << " depthNum: " << depthNum << std::endl;
    }
}
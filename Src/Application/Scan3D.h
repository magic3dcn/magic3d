#pragma once
#include "../Common/AppBase.h"
#include "Scan3DUI.h"
#include "MediaStream.h"
#include "OgreRectangle2D.h"
#include "OgreTexture.h"

namespace MagicApp
{
    class Scan3D : public MagicCore::AppBase
    {
    public:
        Scan3D();
        ~Scan3D();

        virtual bool Enter(void);
        virtual bool Update(float timeElapsed);
        virtual bool Exit(void);

        MediaStream& GetMediaStream();
        bool IsScannerUpdate();
        void StartUpdateScanner();
        void StopUpdateScanner();

    private:
        void Init();
        void InitCanvas(int resolutionX, int resolutionY);
        void DestroyCanvas();
        void SetRenderEnvironment();
        void DestroyRenderEnvironment();
        void UpdateScanner();

    private:
        bool mHasInitilised;
        bool mHasScanStart;
        Scan3DUI mUI;
        MediaStream mMediaStream;
        Ogre::Rectangle2D* mpCanvasColor;
        Ogre::Rectangle2D* mpCanvasDepth;
        Ogre::TexturePtr   mpTexColor;
        Ogre::TexturePtr   mpTexDepth;
    };


}
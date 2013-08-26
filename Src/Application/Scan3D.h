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

        void PlayScanner();
        void Record();
        void PlayRecordScanner();

    private:
        void Init();
        void InitCanvas(int resolutionX, int resolutionY);
        void SetRenderEnvironment();
        void DestroyRenderEnvironment();
        void UpdateScanner();
        void PauseAppState();
        void ResumeAppState();
        void ClearApp();

    private:
        bool mHasInitilised;
        bool mIsScanning;
        bool mIsRecording;
        bool mIsRecordScanning;
        Scan3DUI mUI;
        MediaStream mMediaStream;
        Ogre::Rectangle2D* mpCanvasColor;
        Ogre::Rectangle2D* mpCanvasDepth;
        Ogre::TexturePtr   mpTexColor;
        Ogre::TexturePtr   mpTexDepth;
    };


}
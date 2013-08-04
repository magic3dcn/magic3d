#pragma once
#include "Ogre.h"
#include <string>

namespace MagicCore
{
    class RenderSystem
    {
    private:
        static RenderSystem* mpRenderSys;
        RenderSystem(void);
    public:
        static RenderSystem* GetSingleton(void);
        void Init(void);
        void Update(void);

        Ogre::RenderWindow* GetRenderWindow(void);
        Ogre::SceneManager* GetSceneManager(void);
        Ogre::Camera*       GetMainCamera(void);

        virtual ~RenderSystem(void);

        private:
        Ogre::Root*    mpRoot;
        Ogre::Camera*  mpMainCam;
        Ogre::RenderWindow* mpRenderWin;
        Ogre::SceneManager* mpSceneMgr;
    };
}


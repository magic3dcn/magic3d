#pragma once
#include "Ogre.h"
#include "../DGP/PointCloud3D.h"
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

        void RenderPoint3DSet(MagicDGP::Point3DSet* pPS, std::string psName, std::string psMaterialName);
        void HidePoint3DSet(std::string psName);

    private:
        Ogre::Root*    mpRoot;
        Ogre::Camera*  mpMainCam;
        Ogre::RenderWindow* mpRenderWin;
        Ogre::SceneManager* mpSceneMgr;
    };
}


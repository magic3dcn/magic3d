#pragma once
#include "Ogre.h"
#include "../DGP/PointCloud3D.h"
#include "../DGP/Mesh3D.h"
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

        void RenderPoint3DSet(const MagicDGP::Point3DSet* pPS, std::string psName, std::string psMaterialName);
        void RenderLineSegments(const std::vector<MagicDGP::Vector3>& startPos, const std::vector<MagicDGP::Vector3>& endPos, std::string lsName, std::string materialName);
        void RenderMesh3D(const MagicDGP::Mesh3D* pMesh, std::string meshName, std::string materialName);
        void HideRenderingObject(std::string psName);

    private:
        Ogre::Root*    mpRoot;
        Ogre::Camera*  mpMainCam;
        Ogre::RenderWindow* mpRenderWin;
        Ogre::SceneManager* mpSceneMgr;
    };
}


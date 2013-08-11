#include "StdAfx.h"
#include "RenderSystem.h"
#include "LogSystem.h"
#include "MagicListener.h"

namespace MagicCore
{
	RenderSystem* RenderSystem::mpRenderSys = NULL;

	RenderSystem::RenderSystem(void) : 
        mpRoot(NULL), 
        mpMainCam(NULL), 
        mpRenderWin(NULL), 
        mpSceneMgr(NULL)
    {
    }

    RenderSystem* RenderSystem::GetSingleton()
    {
        if (mpRenderSys == NULL)
        {
            mpRenderSys = new RenderSystem;
        }
        return mpRenderSys;
    }

    void RenderSystem::Init()
    {
        MagicLog << "RenderSystem init...." << std::endl;
        mpRoot = new Ogre::Root();
        if (mpRoot->showConfigDialog())
        {
            // initialise system according to user options.
            mpRenderWin = mpRoot->initialise(true, "Magic3D");
            // Create the scene manager
            mpSceneMgr = mpRoot->createSceneManager(Ogre::ST_GENERIC, "MainSceneManager");
            // Create and initialise the camera
            mpMainCam = mpSceneMgr->createCamera("MainCamera");
            mpMainCam->setPosition(Ogre::Vector3(0,0,4));
            mpMainCam->lookAt(Ogre::Vector3(0,0,0));
            mpMainCam->setNearClipDistance(0.05);
            // Create a viewport covering whole window
            Ogre::Viewport* vp = mpRenderWin->addViewport(mpMainCam);
            vp->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
            // Update the camera aspect ratio to that of the viewport
            mpMainCam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

            mpRoot->addFrameListener(MagicListener::GetSingleton());
            Ogre::WindowEventUtilities::addWindowEventListener(mpRenderWin, MagicListener::GetSingleton());

            //get supported syntax information
            const Ogre::GpuProgramManager::SyntaxCodes &syntaxCodes = Ogre::GpuProgramManager::getSingleton().getSupportedSyntax();
            for (Ogre::GpuProgramManager::SyntaxCodes::const_iterator iter = syntaxCodes.begin();iter != syntaxCodes.end();++iter)
            {
                MagicLog << "supported syntax : " << (*iter) << std::endl;
            }
        }
        else
        {

        }

    }

    void RenderSystem::Update()
    {
        //MagicLog << "    RenderSystem::Update" << std::endl;
        mpRoot->renderOneFrame();
    }

    Ogre::RenderWindow* RenderSystem::GetRenderWindow()
    {
        return mpRenderWin;
    }

    Ogre::SceneManager* RenderSystem::GetSceneManager()
    {
        return mpSceneMgr;
    }

    Ogre::Camera* RenderSystem::GetMainCamera()
    {
        return mpMainCam;
    }

    void RenderSystem::RenderPoint3DSet(MagicDGP::Point3DSet* pPS, std::string psName, std::string psMaterialName)
    {
        Ogre::ManualObject* pMObj = NULL;
        if (mpSceneMgr->hasManualObject(psName))
        {
            pMObj = mpSceneMgr->getManualObject(psName);
            pMObj->clear();
        }
        else
        {
            pMObj = mpSceneMgr->createManualObject(psName);
        }
        pMObj->begin(psMaterialName, Ogre::RenderOperation::OT_POINT_LIST);
        std::map<int, MagicDGP::Point3D*> pointSet = pPS->GetPointSet();
        for (std::map<int, MagicDGP::Point3D*>::iterator itr = pointSet.begin(); itr != pointSet.end(); ++itr)
        {
            MagicDGP::Vector3 pos = itr->second->GetPosition();
            MagicDGP::Vector3 nor = itr->second->GetNormal();
            pMObj->position(pos[0], pos[1], pos[2]);
            pMObj->normal(nor[0], nor[1], nor[2]);
        }
        pMObj->end();
        mpSceneMgr->getRootSceneNode()->attachObject(pMObj);
    }

    void RenderSystem::HidePoint3DSet(std::string psName)
    {
        if (mpSceneMgr->hasManualObject(psName))
        {
            mpSceneMgr->destroyManualObject(psName);
        }
    }

    RenderSystem::~RenderSystem(void)
    {
    }
}


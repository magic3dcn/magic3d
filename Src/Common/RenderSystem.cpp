//#include "StdAfx.h"
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
        bool hasConfig = false;
        if (mpRoot->restoreConfig())
        {
            hasConfig = true;
        }
        else if (mpRoot->showConfigDialog())
        {
            hasConfig = true;
        }

        if (hasConfig)
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
            vp->setBackgroundColour(Ogre::ColourValue(0.8627450980392157, 0.8627450980392157, 0.8627450980392157));
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
            //Exist
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

    void RenderSystem::RenderPoint3DSet(std::string psName, std::string psMaterialName, const MagicDGP::Point3DSet* pPS)
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
            mpSceneMgr->getRootSceneNode()->attachObject(pMObj);
        }
        pMObj->begin(psMaterialName, Ogre::RenderOperation::OT_POINT_LIST);
        int pointNum = pPS->GetPointNumber();
        for (int i = 0; i < pointNum; i++)
        {
            const MagicDGP::Point3D* pPoint = pPS->GetPoint(i);
            MagicDGP::Vector3 pos = pPoint->GetPosition();
            MagicDGP::Vector3 nor = pPoint->GetNormal();
            MagicDGP::Vector3 color = pPoint->GetColor();
            pMObj->position(pos[0], pos[1], pos[2]);
            pMObj->normal(nor[0], nor[1], nor[2]);
            pMObj->colour(color[0], color[1], color[2]);
        }
        pMObj->end();
    }

    void RenderSystem::RenderPoint3DSet(std::string psName, std::string psMaterialName, const MagicDGP::Point3DSet* pPS, const MagicDGP::HomoMatrix4& transform)
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
            mpSceneMgr->getRootSceneNode()->attachObject(pMObj);
        }
        pMObj->begin(psMaterialName, Ogre::RenderOperation::OT_POINT_LIST);
        int pointNum = pPS->GetPointNumber();
        for (int i = 0; i < pointNum; i++)
        {
            const MagicDGP::Point3D* pPoint = pPS->GetPoint(i);
            MagicDGP::Vector3 pos = transform.TransformPoint( pPoint->GetPosition() );
            MagicDGP::Vector3 nor = transform.RotateVector( pPoint->GetNormal() );
            MagicDGP::Vector3 color = pPoint->GetColor();
            pMObj->position(pos[0], pos[1], pos[2]);
            pMObj->normal(nor[0], nor[1], nor[2]);
            pMObj->colour(color[0], color[1], color[2]);
        }
        pMObj->end();
    }

    void RenderSystem::RenderLineSegments(std::string lsName, std::string materialName, const std::vector<MagicDGP::Vector3>& startPos, const std::vector<MagicDGP::Vector3>& endPos)
    {
        Ogre::ManualObject* pMObj = NULL;
        if (mpSceneMgr->hasManualObject(lsName))
        {
            pMObj = mpSceneMgr->getManualObject(lsName);
            pMObj->clear();
        }
        else
        {
            pMObj = mpSceneMgr->createManualObject(lsName);
            mpSceneMgr->getRootSceneNode()->attachObject(pMObj);
        }
        pMObj->begin(materialName, Ogre::RenderOperation::OT_LINE_LIST);
        int lineNum = startPos.size();
        for (int i = 0; i < lineNum; i++)
        {
            MagicDGP::Vector3 start = startPos.at(i);
            MagicDGP::Vector3 end = endPos.at(i);
            pMObj->position(start[0], start[1], start[2]);
            pMObj->position(end[0], end[1], end[2]);
        }
        pMObj->end();
    }

    void RenderSystem::RenderMesh3D(std::string meshName, std::string materialName, const MagicDGP::Mesh3D* pMesh)
    {
        MagicLog << "RenderSystem::RenderMesh3D" << std::endl;
        Ogre::ManualObject* pMObj = NULL;
        if (mpSceneMgr->hasManualObject(meshName))
        {
            pMObj = mpSceneMgr->getManualObject(meshName);
            pMObj->clear();
        }
        else
        {
            pMObj = mpSceneMgr->createManualObject(meshName);
            mpSceneMgr->getRootSceneNode()->attachObject(pMObj);
        }
        pMObj->begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
        int vertNum = pMesh->GetVertexNumber();
        for (int i = 0; i < vertNum; i++)
        {
            const MagicDGP::Vertex3D* pVert = pMesh->GetVertex(i);
            MagicDGP::Vector3 pos = pVert->GetPosition();
            MagicDGP::Vector3 nor = pVert->GetNormal();
            MagicDGP::Vector3 color = pVert->GetColor();
            pMObj->position(pos[0], pos[1], pos[2]);
            pMObj->normal(nor[0], nor[1], nor[2]);
            pMObj->colour(color[0], color[1], color[2]);
        }
        int faceNum = pMesh->GetFaceNumber();
        for (int i = 0; i < faceNum; i++)
        {
            const MagicDGP::Edge3D* pEdge = pMesh->GetFace(i)->GetEdge();
            pMObj->triangle(pEdge->GetVertex()->GetId(), pEdge->GetNext()->GetVertex()->GetId(), pEdge->GetPre()->GetVertex()->GetId());
        }
        pMObj->end();
    }

    void RenderSystem::HideRenderingObject(std::string psName)
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


#include "StdAfx.h"
#include "ViewTool.h"
#include "../Common/RenderSystem.h"
//#include "../Common/MagicOgre.h"

namespace MagicTool
{
    ViewTool::ViewTool() : 
        mMousePos(0, 0, 0),
        mScale(1.f)
    {
    }

    ViewTool::ViewTool(float scale) : 
        mMousePos(0, 0, 0),
        mScale(scale)
    {
    }

    ViewTool::~ViewTool()
    {
    }

    void ViewTool::MousePressed(const OIS::MouseEvent& arg)
    {
        mMousePos = MagicMath::Vector3(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
    }

    void ViewTool::MouseMoved(const OIS::MouseEvent& arg)
    {
        if (arg.state.buttonDown(OIS::MB_Left) && 
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasSceneNode("ModelNode"))
        {
            MagicMath::Vector3 mouseDiff(arg.state.X.abs - mMousePos[0], arg.state.Y.abs - mMousePos[1], 0);
            mMousePos = MagicMath::Vector3(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->yaw(Ogre::Degree(mouseDiff[0]) * 0.2, Ogre::Node::TS_PARENT);
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->pitch(Ogre::Degree(mouseDiff[1]) * 0.2, Ogre::Node::TS_PARENT);
        }
        else if (arg.state.buttonDown(OIS::MB_Right))
        {
            MagicMath::Vector3 mouseDiff(arg.state.X.abs - mMousePos[0], arg.state.Y.abs - mMousePos[1], 0);
            mMousePos = MagicMath::Vector3(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->move(Ogre::Vector3(0, 0, mouseDiff[1]) * 0.007 * mScale);
        }
        else if (arg.state.buttonDown(OIS::MB_Middle))
        {
            MagicMath::Vector3 mouseDiff(arg.state.X.abs - mMousePos[0], arg.state.Y.abs - mMousePos[1], 0);
            mMousePos = MagicMath::Vector3(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->move(Ogre::Vector3(mouseDiff[0] * (-1), mouseDiff[1], 0) * 0.0025 * mScale);
        }
        
    }

    void ViewTool::SetScale(float scale)
    {
        mScale = scale;
    }
}
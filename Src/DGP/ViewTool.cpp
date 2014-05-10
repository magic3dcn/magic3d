#include "StdAfx.h"
#include "ViewTool.h"
#include "../Common/RenderSystem.h"

namespace MagicDGP
{
    ViewTool::ViewTool() : 
        mMousePos(0, 0),
        mScale(1.f)
    {
    }

    ViewTool::ViewTool(float scale) : 
        mMousePos(0, 0),
        mScale(scale)
    {
    }

    ViewTool::~ViewTool()
    {
    }

    void ViewTool::MousePressed(int mousePosX, int mousePosY)
    {
        mMousePos = MagicMath::Vector2(mousePosX, mousePosY);
    }

    void ViewTool::MouseMoved(int mousePosX, int mousePosY, MouseMode mm)
    {
        if (mm == MM_Left_Down && 
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->hasSceneNode("ModelNode"))
        {
            MagicMath::Vector2 mouseDiff(mousePosX - mMousePos[0], mousePosY - mMousePos[1]);
            mMousePos = MagicMath::Vector2(mousePosX, mousePosY);
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->yaw(Ogre::Degree(mouseDiff[0]) * 0.2, Ogre::Node::TS_PARENT);
            MagicCore::RenderSystem::GetSingleton()->GetSceneManager()->getSceneNode("ModelNode")->pitch(Ogre::Degree(mouseDiff[1]) * 0.2, Ogre::Node::TS_PARENT);
        }
        else if (mm == MM_Right_Down)
        {
            MagicMath::Vector2 mouseDiff(mousePosX - mMousePos[0], mousePosY - mMousePos[1]);
            mMousePos = MagicMath::Vector2(mousePosX, mousePosY);
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->move(Ogre::Vector3(0, 0, mouseDiff[1]) * 0.007 * mScale);
        }
        else if (mm == MM_Middle_Down)
        {
            MagicMath::Vector2 mouseDiff(mousePosX - mMousePos[0], mousePosY - mMousePos[1]);
            mMousePos = MagicMath::Vector2(mousePosX, mousePosY);
            MagicCore::RenderSystem::GetSingleton()->GetMainCamera()->move(Ogre::Vector3(mouseDiff[0] * (-1), mouseDiff[1], 0) * 0.0025 * mScale);
        }
        
    }

    void ViewTool::SetScale(float scale)
    {
        mScale = scale;
    }
}
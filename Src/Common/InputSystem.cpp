////#include "StdAfx.h"
#include "InputSystem.h"
#include "LogSystem.h"
#include "MagicListener.h"
//#include "../Common/MagicOgre.h"
#include "OgreRenderWindow.h"

namespace MagicCore
{
    InputSystem* InputSystem::mpInputSys = NULL;

    InputSystem::InputSystem(void) : mpInputMgr(NULL), mpMouse(NULL), mpKeyboard(NULL)
    {
    }

    InputSystem* InputSystem::GetSingleton()
    {
        if (mpInputSys == NULL)
        {
            mpInputSys = new InputSystem;
        }
        return mpInputSys;
    }

    void InputSystem::Init(Ogre::RenderWindow* pWin)
    {
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "InputSystem init....";
        // OIS setup
        OIS::ParamList paramList;
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;

        // get window handle
        pWin->getCustomAttribute("WINDOW", &windowHnd);

        // fill param list
        windowHndStr << (unsigned int)windowHnd;
        paramList.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
        paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
        paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
        paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
        paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));

        // create input system
        mpInputMgr = OIS::InputManager::createInputSystem(paramList);

        mpKeyboard = static_cast<OIS::Keyboard*>(mpInputMgr->createInputObject(OIS::OISKeyboard, true));
        mpKeyboard->setEventCallback(MagicListener::GetSingleton());
        mpKeyboard->setBuffered(true);

        mpMouse = static_cast<OIS::Mouse*>(mpInputMgr->createInputObject(OIS::OISMouse, true));
        mpMouse->setEventCallback(MagicListener::GetSingleton());
        mpMouse->setBuffered(true);

        unsigned int width, height, depth;
        int left, top;

        pWin->getMetrics(width, height, depth, left, top);
        const OIS::MouseState& mouseState = mpMouse->getMouseState();
        mouseState.width = width;
        mouseState.height = height;
        MagicLog(MagicCore::LOGLEVEL_DEBUG) << "OK" << std::endl;
    }

    void InputSystem::Update()
    {
        //MagicLog(MagicCore::LOGLEVEL_DEBUG) << "    InputSystem::Update" << std::endl;
        mpMouse->capture();
        mpKeyboard->capture();
        Ogre::WindowEventUtilities::messagePump();
    }

    void InputSystem::UpdateMouseState(int w, int h)
    {
        mpMouse->getMouseState().width  = w;
        mpMouse->getMouseState().height = h;
    }

    InputSystem::~InputSystem(void)
    {
    }
}
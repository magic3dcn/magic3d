#pragma once
#include "OIS.h"
//#include "../Common/MagicOgre.h"

namespace Ogre
{
    class RenderWindow;
}

namespace MagicCore
{
    class InputSystem
    {
    private:
        static InputSystem* mpInputSys;
        InputSystem(void);
    public:
        static InputSystem* GetSingleton(void);
        void    Init(Ogre::RenderWindow* pWin);
        void    Update();
        void    UpdateMouseState(int w, int h);
        virtual ~InputSystem(void);
    private:
        OIS::InputManager* mpInputMgr;
        OIS::Mouse*        mpMouse;
        OIS::Keyboard*     mpKeyboard;
    };
}

#pragma once
#include "OIS.h"
#include "../DGP/Vector3.h"

namespace MagicTool
{
    class ViewTool
    {
    public:
        ViewTool();
        void MousePressed(const OIS::MouseEvent& arg);
        void MouseMoved(const OIS::MouseEvent& arg);
        ~ViewTool();

    private:
        MagicDGP::Vector3 mMousePos;
    };

}
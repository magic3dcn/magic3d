#pragma once
#include "OIS.h"
#include "../Math/Vector3.h"

namespace MagicTool
{
    class ViewTool
    {
    public:
        ViewTool();
        ViewTool(float scale);
        void MousePressed(const OIS::MouseEvent& arg);
        void MouseMoved(const OIS::MouseEvent& arg);
        void SetScale(float scale);
        ~ViewTool();

    private:
        MagicMath::Vector3 mMousePos;
        float mScale;
    };

}
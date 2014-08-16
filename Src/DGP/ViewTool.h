#pragma once
#include "Math/Vector2.h"

namespace MagicDGP
{
    class ViewTool
    {
    public:
        enum MouseMode
        {
            MM_None,
            MM_Left_Down,
            MM_Middle_Down,
            MM_Right_Down
        };

        ViewTool();
        ViewTool(float scale);
        void MousePressed(int mousePosX, int mousePosY);
        void MouseMoved(int mousePosX, int mousePosY, MouseMode mm);
        void SetScale(float scale);
        ~ViewTool();

    private:
        MagicMath::Vector2 mMousePos;
        float mScale;
    };

}
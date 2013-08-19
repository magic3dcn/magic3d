#include "StdAfx.h"
#include "Scan3D.h"
#include "../Common/LogSystem.h"

namespace MagicApp
{
    Scan3D::Scan3D()
    {
    }

    Scan3D::~Scan3D()
    {
    }

    bool Scan3D::Enter()
    {
        MagicLog << "Enter Scan3D" << std::endl;
        mUI.Setup();
        return true;
    }

    bool Scan3D::Update(float timeElapsed)
    {
        return true;
    }

    bool Scan3D::Exit()
    {
        mUI.Shutdown();
        return true;
    }
}
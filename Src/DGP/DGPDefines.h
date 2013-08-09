#pragma once
#include "../Common/LogSystem.h"

namespace MagicDGP
{
    typedef double Real;
    Real Epsilon = 1.0e-14;
    #define MAGICASSERT(b, m) if (!b) { MagicLog << "Error: " << m << std::endl; }
}
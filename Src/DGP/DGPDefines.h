#pragma once
#include "../Common/LogSystem.h"

namespace MagicDGP
{
    typedef double Real;
    extern Real Epsilon;
    #define MAGICASSERT(b, m) if (!(b)) { ErrorLog << "Error: " << m << std::endl; }
}
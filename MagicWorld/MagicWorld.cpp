// MagicWorld.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../Src/Common/MagicFramework.h"


int _tmain(int argc, _TCHAR* argv[])
{
    MagicCore::MagicFramework magicFrame;
    magicFrame.Init();
    magicFrame.Run();

	return 0;
}


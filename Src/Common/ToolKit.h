#pragma once
#include <string>
#include "../DGP/Vector3.h"

namespace MagicCore
{
    class ToolKit
    {
    private:
        static ToolKit*  mpToolKit;
        ToolKit(void);
    public:
        static ToolKit* GetSingleton(void);
        float GetTime(void);
        bool FileOpenDlg(std::string& selectFileName);
        bool FileSaveDlg(std::string& selectFileName);
        bool IsAppRunning(void);
        void SetAppRunning(bool bRunning);
        bool IsONIInitialized(void);
        void SetONIInitialized(bool bInitialized);
        MagicDGP::Vector3 ColorCoding(float f);

        virtual ~ToolKit(void);

    private:
        bool mAppRunning;
        bool mIsONIInitialized;
	};
}


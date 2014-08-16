#pragma once
#include <string>
#include "Math/Vector3.h"

namespace MagicCore
{
    class ToolKit
    {
    private:
        static ToolKit*  mpToolKit;
        ToolKit(void);
    public:
        static ToolKit* GetSingleton(void);
        static double GetTime(void);
        static bool FileOpenDlg(std::string& selectFileName, char* filterName);
        static bool FileSaveDlg(std::string& selectFileName, char* filterName);
        static MagicMath::Vector3 ColorCoding(float f);
        static void OpenWebsite(std::string& address);

        bool IsAppRunning(void);
        void SetAppRunning(bool bRunning);
        bool IsONIInitialized(void);
        void SetONIInitialized(bool bInitialized);
        void SetMousePressLocked(bool locked);
        bool IsMousePressLocked() const;
        
        virtual ~ToolKit(void);

    private:
        bool mAppRunning;
        bool mIsONIInitialized;
        bool mMousePressLocked; //this is just a hack
	};
}


#pragma once
#include <string>

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

        virtual ~ToolKit(void);

    private:
        bool mAppRunning;
	};
}


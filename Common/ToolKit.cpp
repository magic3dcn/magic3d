#include "StdAfx.h"
#include "ToolKit.h"

namespace MagicCore
{
    ToolKit* ToolKit::mpToolKit = NULL;

    ToolKit::ToolKit(void) : mAppRunning(true)
    {
    }

    ToolKit* ToolKit::GetSingleton()
    {
        if (mpToolKit == NULL)
        {
            mpToolKit = new ToolKit;
        }
        return mpToolKit;
    }

    float ToolKit::GetTime()
    {
        static __int64 start = 0;
        static __int64 frequency = 0;

        if (start == 0)
        {
            QueryPerformanceCounter((LARGE_INTEGER*)&start);
            QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
            return 0.0f;
        }

        __int64 counter = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)&counter);
        return (float) ((counter - start) / double(frequency));
    }

    bool ToolKit::FileOpenDlg(std::string& selectFileName)
    {
        char szFileName[MAX_PATH] = "";
        OPENFILENAME file = { 0 };
        file.lStructSize = sizeof(file);
        file.lpstrFile = szFileName;
        file.nMaxFile = MAX_PATH;
        file.lpstrFilter = "OBJ Files(*.obj)\0*.obj\0TXT Files(*.txt)\0*.txt\0All Files\0*.*\0\0";
        file.nFilterIndex = 1;//默认选择第一个
        //file.Flags = OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR;
        file.Flags = OFN_NOCHANGEDIR;
        // 弹出打开文件的对话框
        if(::GetOpenFileName(&file))
        {
            selectFileName = szFileName;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ToolKit::FileSaveDlg(std::string& selectFileName)
    {
        char szFileName[MAX_PATH] = "";
        OPENFILENAME file = { 0 };
        file.lStructSize = sizeof(file);
        file.lpstrFile = szFileName;
        file.nMaxFile = MAX_PATH;
        file.lpstrFilter = "OBJ Files(*.obj)\0*.obj\0All Files\0*.*\0\0";
        file.nFilterIndex = 1;//默认选择第一个
        // 弹出打开文件的对话框
        if(::GetSaveFileName(&file))
        {
            selectFileName = szFileName;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ToolKit::IsAppRunning()
    {
        return mAppRunning;
    }

    void ToolKit::SetAppRunning(bool bRunning)
    {
        mAppRunning = bRunning;
    }

    ToolKit::~ToolKit(void)
    {
    }
}


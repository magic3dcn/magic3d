#pragma once
#include <Windows.h>
#include <vector>
#include <list>

namespace MagicTool
{
    class Mutex
    {
    public:
        Mutex();
        void Lock();
        void Unlock();
        ~Mutex();

    private:
        CRITICAL_SECTION mCS;
    };

    class ScopedLock
    {
    public:
        ScopedLock(Mutex& mutex);
        ~ScopedLock();

    private:
        Mutex& mMutex;
    };

    class ITask
    {
    public:
        ITask();
        virtual void Run() = 0;
        virtual void OnComplete(void) = 0;
        virtual ~ITask();
    };

    class Thread
    {
    public:
        Thread();
        void Start();
        unsigned Run();
        ~Thread();

    private:
        void* mHandle;
    };

    class ThreadPool
    {
    public:
        ThreadPool(int threadCount);
        void InsertTask(ITask* pTask);
        void WaitUntilAllDone();
        ~ThreadPool();

    private:
        int mThreadCount;
        std::vector<Thread*> mThreadList;
        std::list<ITask*> mTaskList;
        Mutex mLock;
    };
}
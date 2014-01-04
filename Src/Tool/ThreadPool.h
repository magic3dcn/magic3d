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

        friend class ConditionVariable;
    };

    class ScopedLock
    {
    public:
        ScopedLock(Mutex& mutex);
        ~ScopedLock();

    private:
        Mutex& mMutex;
    };

    class ConditionVariable
    {
    public:
        ConditionVariable();
        void Sleep(Mutex& mutex);
        void WakeSingle();
        void WakeAll();
        ~ ConditionVariable();

    private:
        CONDITION_VARIABLE mCV;
    };

    enum TaskType
    {
        TP_Exit = 0,
        TP_Normal
    };

    class ITask
    {
    public:
        ITask(TaskType tp = TP_Normal);
        virtual void Run();
        virtual void OnComplete(void);
        TaskType GetType() const;
        virtual ~ITask();

    private:
        TaskType mTP;
    };

    class ThreadPool;
    class Thread
    {
    public:
        Thread(ThreadPool* pTP);
        void Start();
        unsigned Run();
        ~Thread();

    private:
        ThreadPool* mpThreadPool;
    };

    class ThreadPool
    {
    public:
        ThreadPool(int threadCount);
        void InsertTask(ITask* pTask);
        ITask* GetTask(void);
        void FinishATask(void);
        void WaitUntilAllDone();
        ~ThreadPool();

    private:
        int mThreadCount;
        std::vector<Thread*> mThreadList;
        std::list<ITask*> mTaskList;
        Mutex mMutex;
        ConditionVariable mGetTaskCV;
        ConditionVariable mFinishTaskCV;
        int mTaskLeftCount;
    };
}
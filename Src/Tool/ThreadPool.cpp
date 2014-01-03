#include "ThreadPool.h"
#include <process.h>

namespace MagicTool
{
    Mutex::Mutex()
    {
    }

    void Mutex::Lock()
    {
        EnterCriticalSection(&mCS);
    }

    void Mutex::Unlock()
    {
        LeaveCriticalSection(&mCS);
    }

    Mutex::~Mutex()
    {
    }

    ScopedLock::ScopedLock(Mutex& mutex) :
        mMutex(mutex)
    {
        mMutex.Lock();
    }

    ScopedLock::~ScopedLock()
    {
        mMutex.Unlock();
    }

    ITask::ITask()
    {
    }

    ITask::~ITask()
    {
    }

    static unsigned __stdcall RunThread(void *arg)
    {
        Thread *thread = (Thread *)arg;
        unsigned result = thread->Run();
        return result;
    }

    Thread::Thread()
    {
    }

    void Thread::Start()
    {
        mHandle = reinterpret_cast<void *>(_beginthreadex(NULL, 0, RunThread, (void *)this, 0, NULL));
    }

    unsigned Thread::Run()
    {
        return 0;
    }

    Thread::~Thread()
    {
    }

    ThreadPool::ThreadPool(int threadCount) :
        mThreadCount(threadCount),
        mThreadList(),
        mTaskList(),
        mLock()
    {
        for (int tid = 0; tid < mThreadCount; tid++)
        {
            Thread* pThread = new Thread();
            pThread->Start();
            mThreadList.push_back(pThread);
        }
    }

    ThreadPool::~ThreadPool()
    {
    }

    void ThreadPool::InsertTask(ITask* pTask)
    {
        ScopedLock scopedLock(mLock);
        mTaskList.push_back(pTask);
    }

    void ThreadPool::WaitUntilAllDone()
    {

    }
}
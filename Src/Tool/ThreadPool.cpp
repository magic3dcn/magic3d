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

    ConditionVariable::ConditionVariable() :
        mCV()
    {
    }

    ConditionVariable::~ConditionVariable()
    {
    }

    void ConditionVariable::Sleep(Mutex& mutex)
    {
        SleepConditionVariableCS(&mCV, &(mutex.mCS), INFINITE);
    }

    void ConditionVariable::WakeSingle()
    {
        WakeConditionVariable(&mCV);
    }

    void ConditionVariable::WakeAll()
    {
        WakeAllConditionVariable(&mCV);
    }

    ITask::ITask(TaskType tp) : 
        mTP(tp)
    {
    }

    void ITask::Run()
    {

    }

    void ITask::OnComplete(void)
    {
        delete this;
    }

    TaskType ITask::GetType() const
    {
        return mTP;
    }

    ITask::~ITask()
    {
    }

    static unsigned __stdcall RunThread(void *arg)
    {
        Thread *thread = (Thread *)arg;
        unsigned result = thread->Run();
        _endthreadex(0);
        return result;
    }

    Thread::Thread(ThreadPool* pTP) : 
        mpThreadPool(pTP)
    {
    }

    void Thread::Start()
    {
        _beginthreadex(NULL, 0, RunThread, (void *)this, 0, NULL);
    }

    unsigned Thread::Run()
    {
        while (true)
        {
            ITask* pTask = mpThreadPool->GetTask();
            if (pTask != NULL)
            {
                if (pTask->GetType() == TP_Exit)
                {
                    pTask->OnComplete();
                    mpThreadPool->FinishATask();
                    break;
                }
                else if (pTask->GetType() == TP_Normal)
                {
                    pTask->Run();
                    pTask->OnComplete();
                    mpThreadPool->FinishATask();
                }
            }
        }
        return 0;
    }

    Thread::~Thread()
    {
    }

    ThreadPool::ThreadPool(int threadCount) :
        mThreadCount(threadCount),
        mThreadList(),
        mTaskList(),
        mMutex(),
        mGetTaskCV(),
        mFinishTaskCV(),
        mTaskLeftCount(0)
    {
        for (int tid = 0; tid < mThreadCount; tid++)
        {
            Thread* pThread = new Thread(this);
            pThread->Start();
            mThreadList.push_back(pThread);
        }
    }

    ThreadPool::~ThreadPool()
    {
        for (int tid = 0; tid < mThreadCount; tid++)
        {
            ITask* pExitTask = new ITask(TP_Exit);
            InsertTask(pExitTask);
        }
        WaitUntilAllDone();
        for (std::vector<Thread*>::iterator itr = mThreadList.begin(); itr != mThreadList.end(); itr++)
        {
            if ((*itr) != NULL)
            {
                delete (*itr);
                (*itr) = NULL;
            }
        }
        mThreadList.clear();
    }

    void ThreadPool::InsertTask(ITask* pTask)
    {
        ScopedLock scopedLock(mMutex);
        mTaskList.push_back(pTask);
        mTaskLeftCount++;
        mGetTaskCV.WakeSingle();
    }

    ITask* ThreadPool::GetTask(void)
    {
        ScopedLock scopedLock(mMutex);
        while (mTaskList.size() == 0)
        {
            mGetTaskCV.Sleep(mMutex);
        }
        ITask* pTask = mTaskList.front();
        mTaskList.pop_front();
        return pTask;
    }

    void ThreadPool::FinishATask(void)
    {
        ScopedLock scopedLock(mMutex);
        mTaskLeftCount--;
        mFinishTaskCV.WakeSingle();
    }

    void ThreadPool::WaitUntilAllDone()
    {
        while (true)
        {
            ScopedLock scopedLock(mMutex);
            if (mTaskLeftCount > 0)
            {
                mFinishTaskCV.Sleep(mMutex);
            }
            else
            {
                break;
            }
        }
    }
}
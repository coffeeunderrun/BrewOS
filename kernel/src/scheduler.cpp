#include <process.hpp>
#include <scheduler.hpp>
#include <thread.hpp>

#define MAX_PRIORITY 3

extern "C" void switch_thread(BrewOS::Scheduler::Thread *from, BrewOS::Scheduler::Thread *to);

namespace BrewOS::Scheduler
{
    static Process s_kernelProcess;
    static Thread *s_threadKernel = *s_kernelProcess.GetThreads().begin(),
                  *s_threadCurrent = nullptr;

    static LinkedList<Thread *> s_threadQueue[MAX_PRIORITY];

    void Start()
    {
        for (auto i = 0; true; i = i >= MAX_PRIORITY ? 0 : i + 1)
        {
            for (auto thread : s_threadQueue[i])
            {
                s_threadCurrent = thread;
                thread->SetState(ThreadState::Active);
                switch_thread(s_threadKernel, thread);
                thread->SetState(ThreadState::Ready);
            }
        }
    }

    void Yield()
    {
        switch_thread(s_threadCurrent, s_threadKernel);
    }

    void AddProcess(Entry entry)
    {
        s_kernelProcess.AddChild(0, 0, entry);
    }

    void OnThreadStateChange(Thread *thread)
    {
        if (thread == s_threadKernel)
        {
            return;
        }

        switch (thread->GetState())
        {
        case ThreadState::Ready:
            s_threadQueue[thread->GetPriority()].Add(thread);
            break;

        default:
            for (auto i = 0; i < MAX_PRIORITY; i++)
            {
                s_threadQueue[i].Remove(thread);
            }
            break;
        }
    }
}

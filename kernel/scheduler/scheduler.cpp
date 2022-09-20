#include "process.hpp"
#include "scheduler.hpp"
#include "thread.hpp"

extern "C" void _switch(BrewOS::Scheduler::Thread *from, BrewOS::Scheduler::Thread *to);

namespace BrewOS::Scheduler
{
    static LinkedList<Process *> s_processes;
    static Thread *s_threadKernel = nullptr,
                  *s_threadCurrent = nullptr;

    void Start()
    {
        Process *kernelProcess = new Process(0, 1);
        AddProcess(kernelProcess);
        kernelProcess->AddThread(s_threadKernel = new Thread());

        while (true)
        {
            for (Process *process : s_processes)
            {
                for (Thread *thread : process->GetThreads())
                {
                    s_threadCurrent = thread;
                    _switch(s_threadKernel, thread);
                }
            }
        }
    }

    void Switch()
    {
        _switch(s_threadCurrent, s_threadKernel);
    }

    void AddProcess(Process *process)
    {
        s_processes.Add(process);
    }
}

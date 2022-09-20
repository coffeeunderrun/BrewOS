#ifndef BREWOS_SCHEDULER_THREAD_H
#define BREWOS_SCHEDULER_THREAD_H

#include "entry.hpp"
#include <registers.hpp>
#include <stdint.h>

namespace BrewOS::Scheduler
{
    class Process;

    class Thread
    {
    private:
        // Keep at top of class for _switch function
        uint64_t m_stackPointer = 0,
                 m_pageMap = 0;

        uint64_t *m_stack = nullptr;

        Process *m_process = nullptr;

        friend class Process;

    public:
        Thread();
        Thread(Entry entry);
        ~Thread();
    };
}

#endif // BREWOS_SCHEDULER_THREAD_H

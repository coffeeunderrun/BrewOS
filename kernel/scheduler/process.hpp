#ifndef BREWOS_SCHEDULER_PROCESS_H
#define BREWOS_SCHEDULER_PROCESS_H

#include "entry.hpp"
#include <linkedlist.hpp>
#include <stddef.h>
#include <stdint.h>

namespace BrewOS::Scheduler
{
    class Thread;

    class Process
    {
    private:
        uint32_t m_id;

        uint8_t m_priority;

        LinkedList<Thread *> m_threads;

    public:
        Process(uint32_t id, uint8_t priority);
        Process(uint32_t id, uint8_t priority, Entry entry);
        ~Process();

        LinkedList<Thread *> &GetThreads();

        void AddThread(Thread *thread);

        void RemoveThread(Thread *thread);
    };
}

#endif // BREWOS_SCHEDULER_PROCESS_H

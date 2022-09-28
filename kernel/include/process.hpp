#ifndef BREWOS_PROCESS_H
#define BREWOS_PROCESS_H

#include <linkedlist.hpp>
#include <scheduler.hpp>
#include <stddef.h>
#include <stdint.h>

namespace BrewOS::Scheduler
{
    class Thread;

    typedef enum class ProcessState
    {
        Ready,
        Active
    } ProcessState;

    class Process
    {
    private:
        uint32_t m_id;

        uint8_t m_priority;

        Process *m_parent = nullptr;

        ProcessState m_state;

        LinkedList<Process *> m_children;

        LinkedList<Thread *> m_threads;

    public:
        Process();
        Process(uint32_t id, uint8_t priority, Entry entry);
        ~Process();

        void AddChild(uint32_t id, uint8_t priority, Entry entry);

        void RemoveChild(uint32_t id);

        LinkedList<Process *> &GetChildren()
        {
            return m_children;
        }

        LinkedList<Thread *> &GetThreads()
        {
            return m_threads;
        }

        void AddThread(Thread *thread);

        void RemoveThread(Thread *thread);
    };
}

#endif // BREWOS_PROCESS_H

#ifndef BREWOS_THREAD_H
#define BREWOS_THREAD_H

#include <registers.hpp>
#include <scheduler.hpp>
#include <stdint.h>

namespace BrewOS::Scheduler
{
    class Process;

    typedef enum class ThreadState
    {
        Ready,
        Active,
        Waiting,
        Deleted
    } ThreadState;

    class Thread
    {
    private:
        // Keep at top of class for _switch function
        uint64_t m_stackPointer = 0,
                 m_pageMap = 0;

        uint8_t m_priority = 0;

        uint64_t *m_stack = nullptr;

        Process *m_process = nullptr;

        ThreadState m_state;

        friend class Process;

    public:
        Thread();
        Thread(Entry entry);
        ~Thread();

        void SetPriority(uint8_t priority)
        {
            m_priority = priority;
        }

        uint8_t GetPriority()
        {
            return m_priority;
        }

        void SetState(ThreadState state);

        ThreadState GetState()
        {
            return m_state;
        }
    };
}

#endif // BREWOS_THREAD_H

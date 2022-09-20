#include "process.hpp"
#include "thread.hpp"

namespace BrewOS::Scheduler
{
    Process::Process(uint32_t id, uint8_t priority)
        : m_id(id), m_priority(priority)
    {
    }

    Process::Process(uint32_t id, uint8_t priority, Entry entry)
        : Process(id, priority)
    {
        AddThread(new Thread(entry));
    }

    LinkedList<Thread *> &Process::GetThreads()
    {
        return m_threads;
    }

    void Process::AddThread(Thread *thread)
    {
        m_threads.Add(thread);
    }

    void Process::RemoveThread(Thread *thread)
    {
        m_threads.Remove(thread);
    }
}

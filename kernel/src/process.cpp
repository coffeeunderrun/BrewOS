#include <process.hpp>
#include <thread.hpp>

namespace BrewOS::Scheduler
{
    Process::Process() : m_id(0), m_priority(0)
    {
        AddThread(new Thread());
    }

    Process::Process(uint32_t id, uint8_t priority, Entry entry)
        : m_id(id), m_priority(priority)
    {
        AddThread(new Thread(entry));
    }

    Process::~Process()
    {
        for (auto thread : m_threads)
        {
            delete thread;
        }

        for (auto child : m_children)
        {
            delete child;
        }
    }

    void Process::AddChild(uint32_t id, uint8_t priority, Entry entry)
    {
        Process *process = new Process(id, priority, entry);
        process->m_parent = this;
        m_children.Add(process);
    }

    void Process::RemoveChild(uint32_t id)
    {
        for (auto child : m_children)
        {
            if (child->m_id == id)
            {
                m_children.Remove(child);
                delete child;
            }
        }
    }

    void Process::AddThread(Thread *thread)
    {
        m_threads.Add(thread);
        thread->SetState(ThreadState::Ready);
    }

    void Process::RemoveThread(Thread *thread)
    {
        thread->SetState(ThreadState::Deleted);
        m_threads.Remove(thread);
    }
}

#include "process.hpp"
#include "thread.hpp"

#if ARCH == x86_64
#define KERNEL_STACK_SIZE 512
#define KERNEL_STACK_RETURN KERNEL_STACK_SIZE - 1
#define KERNEL_STACK_POINTER KERNEL_STACK_SIZE - 8
#endif

namespace BrewOS::Scheduler
{
    Thread::Thread()
    {
    }
    
    Thread::Thread(Entry entry)
    {
        m_stack = new uint64_t[KERNEL_STACK_SIZE];
        m_stack[KERNEL_STACK_RETURN] = reinterpret_cast<uintptr_t>(entry);
        m_stackPointer = reinterpret_cast<uintptr_t>(&m_stack[KERNEL_STACK_POINTER]);
    }

    Thread::~Thread()
    {
        delete m_stack;
    }
}

#include "memory/heap.hpp"
#include <stddef.h>

typedef int Guard __attribute__((mode(__DI__)));

extern "C"
{
    int __cxa_guard_acquire(Guard *guard)
    {
        return *reinterpret_cast<char *>(guard) == 0;
    }

    void __cxa_guard_release(Guard *guard)
    {
        *reinterpret_cast<char *>(guard) = 1;
    }

    void __cxa_guard_abort(Guard *)
    {
    }

    void __cxa_pure_virtual()
    {
    }
}

void *operator new(size_t size)
{
    return BrewOS::Memory::Heap::Allocate(size);
}

void *operator new[](size_t size)
{
    return BrewOS::Memory::Heap::Allocate(size);
}

void operator delete(void *address)
{
    BrewOS::Memory::Heap::Free(address);
}

void operator delete(void *address, size_t size)
{
    BrewOS::Memory::Heap::Free(address);
}

void operator delete[](void *address)
{
    BrewOS::Memory::Heap::Free(address);
}

void operator delete[](void *address, size_t size)
{
    BrewOS::Memory::Heap::Free(address);
}

#include "memory/heap.hpp"
#include <stddef.h>

typedef int __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire(__guard *guard)
{
    return *reinterpret_cast<char *>(guard) == 0;
}

extern "C" void __cxa_guard_release(__guard *guard)
{
    *reinterpret_cast<char *>(guard) = 1;
}

extern "C" void __cxa_guard_abort(__guard *)
{
}

extern "C" void __cxa_pure_virtual()
{
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

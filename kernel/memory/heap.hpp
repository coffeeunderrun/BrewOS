#ifndef BREWOS_MEMORY_HEAP_H
#define BREWOS_MEMORY_HEAP_H

#include <stddef.h>
#include <stdint.h>

namespace BrewOS::Memory::Heap
{
    void *Allocate(const size_t size);
 
    void *Allocate(const size_t count, const size_t size);
 
    void *Resize(void *addr, const size_t size);
 
    void Free(void *addr);
}

#endif // BREWOS_MEMORY_HEAP_H

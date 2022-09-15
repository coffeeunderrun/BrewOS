#include "memory.hpp"

/*** DO NOT USE THESE AFTER INITIALIZATION ***/
extern const void *_kernel_start;    // Start of kernel in physical address space
extern const uint32_t _kernel_pages; // Number of contiguous pages from start of kernel
extern const BrewOS::Memory::MemoryMap *_memory_map;

namespace BrewOS::Memory
{
    void Initialize()
    {
    }

    void Zero(void *addr, const size_t size)
    {
        uint8_t *p = static_cast<uint8_t *>(addr);

        for (size_t i = 0; i < size; i++)
        {
            *p++ = 0;
        }
    }
}

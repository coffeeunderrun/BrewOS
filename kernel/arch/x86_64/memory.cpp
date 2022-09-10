#include <memory.hpp>

/*** DO NOT USE THESE AFTER INITIALIZATION ***/
extern const void *_kernel_start;    // Start of kernel in physical address space
extern const uint64_t _kernel_pages; // Number of contiguous pages from start of kernel
extern const BrewOS::Memory::MemoryMap *_memory_map;

namespace BrewOS::Memory
{
    void Initialize()
    {
    }
}

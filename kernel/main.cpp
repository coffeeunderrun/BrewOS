#include <memory.hpp>

using BrewOS::Memory::MemoryMap;

namespace BrewOS
{
    extern "C" void KernelInit(void *pKernelTop, uint64_t nKernelPages, MemoryMap *pMemoryMap)
    {
    }

    extern "C" void KernelMain()
    {
    }
}

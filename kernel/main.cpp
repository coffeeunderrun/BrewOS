#include <graphics.hpp>
#include <memory.hpp>

using BrewOS::Graphics::GraphicsOutput;
using BrewOS::Memory::MemoryMap;

namespace BrewOS
{
    extern "C" void KernelInit(void *kernelTop, uint64_t kernelPages, MemoryMap *memoryMap, GraphicsOutput *graphicsOutput)
    {
        Graphics::Initialize(graphicsOutput);
    }

    extern "C" void KernelMain()
    {
        Graphics::ClearScreen(0x6495ED);
    }
}

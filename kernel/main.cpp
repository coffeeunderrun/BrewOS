#include <graphics.hpp>
#include <memory.hpp>

namespace BrewOS
{
    extern "C" void KernelInit()
    {
        Graphics::Initialize();

        // Make sure this is done last as data in lower-half from UEFI will be lost
        Memory::Initialize();
    }

    extern "C" void KernelMain()
    {
        Graphics::ClearScreen(0x6495ED);
        Graphics::DrawText(L"Welcome to BrewOS!", 0, 0, 0xFFFFFF);
    }
}

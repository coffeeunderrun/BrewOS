#include <interrupts.hpp>
#include <graphics.hpp>
#include <memory.hpp>

// TODO: font should be moved to higher-half
extern const BrewOS::Graphics::PsfFont *font;

namespace BrewOS
{
    extern "C" void KernelInit()
    {
        Interrupts::Initialize();
        Graphics::Initialize();

        // Make sure this is done last as data in lower-half from UEFI will be lost
        Memory::Initialize();
    }

    extern "C" __attribute__((noreturn)) void KernelMain()
    {
        Graphics::ClearScreen(Graphics::Color::CornflowerBlue);
        Graphics::DrawText(L"Welcome to BrewOS!", 0, 0, Graphics::Color::White, Graphics::Color::Transparent, *font);

        while (true)
            ;
    }
}

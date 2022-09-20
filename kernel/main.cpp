#include "interrupts.hpp"
#include "graphics/graphics.hpp"
#include "memory/memory.hpp"

// TODO: font should be moved to higher-half
extern const BrewOS::Graphics::PsfFont *_font;

namespace BrewOS
{
    extern "C" void KernelInit()
    {
        Interrupts::Initialize();
        Graphics::Initialize();

        // Make sure this is done last as data in lower-half from UEFI will be lost
        Memory::Initialize();
    }

    extern "C" void KernelMain()
    {
        Graphics::ClearScreen(Graphics::Color::CornflowerBlue);
        Graphics::DrawText(L"Welcome to BrewOS!", 0, 0, Graphics::Color::White, Graphics::Color::Transparent, *_font);
    }
}

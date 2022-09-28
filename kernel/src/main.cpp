#include <interrupts.hpp>
#include <graphics.hpp>
#include <memory.hpp>
#include <process.hpp>
#include <scheduler.hpp>
#include <thread.hpp>

// TODO: font should be moved to higher-half
extern const BrewOS::Graphics::PsfFont *font;

namespace BrewOS
{
    void Task1()
    {
        while (true)
        {
            Graphics::DrawText(L"Task 1", 0, 20, Graphics::Color::Green, Graphics::Color::Black, *font);
            for (uint64_t i = 0; i < 0xFFFFFFF; i++)
                ;
            Scheduler::Yield();
        }
    }

    void Task2()
    {
        while (true)
        {
            Graphics::DrawText(L"Task 2", 0, 20, Graphics::Color::Red, Graphics::Color::Black, *font);
            for (uint64_t i = 0; i < 0xFFFFFFF; i++)
                ;
            Scheduler::Yield();
        }
    }

    extern "C" void KernelInit()
    {
        Interrupts::Initialize();
        Graphics::Initialize();

        // Make sure this is done last as data in lower-half from UEFI will be lost
        Memory::Initialize();
    }

    extern "C" __attribute__((noreturn)) void KernelMain()
    {
        Graphics::ClearScreen(Graphics::Color::Black);
        Graphics::DrawText(L"Welcome to BrewOS!", 0, 0, Graphics::Color::Green, Graphics::Color::Black, *font);

        Scheduler::AddProcess(Task1);
        Scheduler::AddProcess(Task2);
        Scheduler::Start();

        while (true)
            ;
    }
}

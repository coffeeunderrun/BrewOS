#include <interrupts.hpp>
#include <arch/cpu.hpp>

extern "C" void _init_interrupts();

namespace BrewOS::Interrupts
{
    void Initialize()
    {
        _init_interrupts();
    }

    extern "C" void InterruptHandler(uint64_t vector, CPU::Registers *registers)
    {
    }

    extern "C" void ErrorHandler(uint64_t vector, uint64_t error, CPU::Registers *registers)
    {
    }
}

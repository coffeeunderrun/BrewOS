#include <interrupts.hpp>
#include <arch/cpu.hpp>

extern "C" void _init_interrupts();

namespace BrewOS::Interrupts
{
    void Initialize()
    {
        _init_interrupts();
    }

    extern "C" void InterruptHandler(uint64_t vector, uint64_t code, CPU::Registers *registers)
    {
    }
}

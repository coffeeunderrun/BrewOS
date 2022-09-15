#include "interrupts.hpp"
#include <stdint.h>

extern "C" void _init_interrupts();

namespace BrewOS::CPU
{
    typedef struct Registers Registers;
}

namespace BrewOS::Interrupts
{
    typedef void (*Callback)(CPU::Registers *registers);

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

    void AddCallback(uint64_t vector, Callback callback)
    {
    }

    void RemoveCallback(uint64_t vector, Callback callback)
    {
    }
}

#ifndef BREWOS_INTERRUPTS_H
#define BREWOS_INTERRUPTS_H

#include <registers.hpp>

namespace BrewOS::Interrupts
{
    typedef void (*Callback)(uint64_t error, Registers *registers);

    void Initialize();

    void AddCallback(uint64_t vector, Callback callback);

    void RemoveCallback(uint64_t vector, Callback callback);
}

#endif // BREWOS_INTERRUPTS_H

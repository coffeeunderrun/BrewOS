#include "interrupts.hpp"
#include "vector.hpp"
#include <stddef.h>
#include <stdint.h>

#if ARCH == x86_64
#define INTERRUPT_VECTOR_MAX 48
#else
#define INTERRUPT_VECTOR_MAX 256
#endif

extern "C" void _init_interrupts();

namespace BrewOS::CPU
{
    typedef struct Registers Registers;
}

namespace BrewOS::Interrupts
{
    typedef void (*Callback)(uint64_t error, CPU::Registers *registers);

    static Vector<Callback> *s_callbacks[INTERRUPT_VECTOR_MAX];

    void Initialize()
    {
        _init_interrupts();
    }

    void AddCallback(uint64_t vector, Callback callback)
    {
        if (vector >= INTERRUPT_VECTOR_MAX)
        {
            return;
        }

        if (s_callbacks[vector] == nullptr)
        {
            s_callbacks[vector] = new Vector<Callback>();
        }

        s_callbacks[vector]->Push(callback);
    }

    void RemoveCallback(uint64_t vector, Callback callback)
    {
        if (vector >= INTERRUPT_VECTOR_MAX)
        {
            return;
        }

        for (size_t i = 0; i < s_callbacks[vector]->GetCount(); i++)
        {
            if (s_callbacks[vector]->Get(i) == callback)
            {
                // Decrement index as removing an item shifts subsequent items
                s_callbacks[vector]->Remove(i--);
            }
        }
    }

    extern "C" void InterruptHandler(uint64_t vector, uint64_t error, CPU::Registers *registers)
    {
        for (Callback callback : *s_callbacks[vector])
        {
            callback(error, registers);
        }
    }
}

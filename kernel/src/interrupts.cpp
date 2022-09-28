#include <interrupts.hpp>
#include <registers.hpp>
#include <stddef.h>
#include <stdint.h>
#include <vector.hpp>
#include <vectors.hpp>

extern "C" void init_interrupts();

namespace BrewOS::Interrupts
{
    typedef void (*Callback)(uint64_t error, Registers *registers);

    static Vector<Callback> *s_callbacks[INTERRUPT_VECTOR_MAX];

    void Initialize()
    {
        init_interrupts();
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

    extern "C" void InterruptHandler(uint64_t vector, uint64_t error, Registers *registers)
    {
        if (s_callbacks[vector] == nullptr)
        {
            return;
        }

        for (Callback callback : *s_callbacks[vector])
        {
            callback(error, registers);
        }
    }
}

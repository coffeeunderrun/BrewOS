#ifndef BREWOS_ARCH_INTERRUPTS_H
#define BREWOS_ARCH_INTERRUPTS_H

#include "cpu.hpp"

namespace BrewOS::Interrupts
{
    namespace Vectors
    {
        const uint64_t DIVIDE_BY_ZERO_ERROR = 0;
        const uint64_t DEBUG = 1;
        const uint64_t NMI = 2;
        const uint64_t BREAKPOINT = 3;
        const uint64_t OVERFLOW = 4;
        const uint64_t BOUND_RANGE_EXCEEDED = 5;
        const uint64_t INVALID_OPCODE = 6;
        const uint64_t DEVICE_NOT_AVAILABLE = 7;
        const uint64_t DOUBLE_FAULT = 8;
        // 9 is unsupported
        const uint64_t INVALID_TSS = 10;
        const uint64_t SEGMENT_NOT_PRESENT = 11;
        const uint64_t STACK_SEGMENT_FAULT = 12;
        const uint64_t GENERAL_PROTECTION_FAULT = 13;
        const uint64_t PAGE_FAULT = 14;
        // 15 is reserved
        const uint64_t X87_FLOATING_POINT_EXCEPTION = 16;
        const uint64_t ALIGNMENT_CHECK = 17;
        const uint64_t MACHINE_CHECK = 18;
        const uint64_t SIMD_FLOATING_POINT_EXCEPTION = 19;
        // 20 is reserved
        const uint64_t CONTROL_PROTECTION_EXCEPTION = 21;
        // 22-27 are reserved
        const uint64_t HYPERVISOR_INJECTION_EXCEPTION = 28;
        const uint64_t VMM_COMMUNICATION_EXCEPTION = 29;
        const uint64_t SECURITY_EXCEPTION = 30;
        // 31 is reserved
        const uint64_t PIT = 32;
        const uint64_t KEYBOARD = 33;
        const uint64_t PIC_CASCADE = 34;
        const uint64_t COM_2 = 35;
        const uint64_t COM_1 = 36;
        const uint64_t LPT_2 = 37;
        const uint64_t FDD_CONTROLLER = 38;
        const uint64_t LPT_1 = 39;
        const uint64_t RTC = 40;
        // 41-43 are unassigned
        const uint64_t MOUSE = 44;
        const uint64_t FPU = 45;
        const uint64_t HDD_CONTROLLER_1 = 46;
        const uint64_t HDD_CONTROLLER_2 = 47;
    }

    typedef void (*Callback)(CPU::Registers *registers);

    // Implementations in kernel
    extern void AddCallback(uint64_t vector, Callback callback);
    extern void RemoveCallback(uint64_t vector, Callback callback);
}

#endif // BREWOS_ARCH_INTERRUPTS_H

bits 64

global initialize_idt, yield

extern gdt.kernel_cs, ist.rsp0
extern get_current_process, get_next_process, swap_process

%define PIC1_CTRL_PORT 0x20
%define PIC1_DATA_PORT 0x21
%define PIC2_CTRL_PORT 0xA0
%define PIC2_DATA_PORT 0xA1
%define PIC_END_IRQ    0x20

%define ICW1        0x11
%define ICW2_MASTER 0x20
%define ICW2_SLAVE  0x28
%define ICW3_MASTER 0x04
%define ICW3_SLAVE  0x02
%define ICW4        0x01

%define IDT_PRESENT 0x80   ; Present
%define IDT_RING(n) n << 5 ; Privilege level (ring-n)
%define IDT_INT     0x0E   ; Interrupt gate
%define IDT_TRAP    0x0F   ; Trap gate

; ISR isr, handler, irq, [code]
%macro ISR_ENTRY 3-4
.%1:
%if %0 = 4
    push %4 ; Code (exception error code, original hardware IRQ)
%endif
    push %3 ; IRQ vector
    jmp %2
%endmacro

; IDT_ENTRY isr, ist, flags, segment
%macro IDT_ENTRY 4
    dw isr.%1 - isr ; ISR offset 0..15
    dw %4           ; Segment selector
    db %2           ; IST offset
    db %3           ; Flags
    dw 0            ; ISR offset 16..31
    dd 0            ; ISR offset 32..63
    dd 0            ; Reserved
%endmacro

section .text

initialize_idt:
    mov al, ICW1 ; Initialize programmable interrupt controllers
    out PIC1_CTRL_PORT, al
    out PIC2_CTRL_PORT, al

    mov al, ICW2_MASTER ; Remap PICs as first 32 interrupts are reserved
    out PIC1_DATA_PORT, al
    mov al, ICW2_SLAVE
    out PIC2_DATA_PORT, al

    mov al, ICW3_MASTER ; Cascading
    out PIC1_DATA_PORT, al
    mov al, ICW3_SLAVE
    out PIC2_DATA_PORT, al

    mov al, ICW4 ; Environment info
    out PIC1_DATA_PORT, al
    out PIC2_DATA_PORT, al

    xor al, al ; Mask interrupts
    out PIC1_DATA_PORT, al
    out PIC2_DATA_PORT, al

    call update_idt_entries

    lea rax, idt.pointer
    lidt [rax]
    ret

; Tell the PIC that the interrupt has been handled
; Interrupt vector to be cleared in AL
; Vectors 32-39, originally 0-7, are PIC1
; Vectors 40-47, originally 8-15, are PIC2
acknowledge:
    cmp al, 32 ; Ensure vector is >= 32
    jb .done
    cmp al, 47 ; Ensure vector is <= 47
    ja .done
    cmp al, 40 ; Which PIC should be informed?
    mov al, PIC_END_IRQ
    jge .pic2  ; When vector is >= 40 select PIC2
    out PIC1_CTRL_PORT, al
    ret
.pic2:
    out PIC2_CTRL_PORT, al
.done:
    ret

update_idt_entries:
    mov rcx, idt.count ; Get number of IDT entries
    mov rdi, idt

.loop:
    test byte [rdi + 5], 0xF ; Check if entry is present
    jnz .update

    add rdi, 16 ; Skip to next entry if not present
    jmp .skip

.update:
    movzx rax, word [rdi] ; Get ISR offset from IDT entry
    add rax, isr          ; Add ISR base
    stosw                 ; Update entry offset 0..15
    add rdi, 4            ; Skip over segment, flags, and IST
    shr rax, 16
    stosw                 ; Update entry offset 16..31
    shr rax, 16
    stosd                 ; Update entry offset 32..63
    add rdi, 4            ; Skip over reserved

.skip:
    loop .loop
    ret

handler:
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rsi
    push rdi
    push rbp
    push rdx
    push rcx
    push rbx
    push rax

    mov rax, [rsp + 8 * 15] ; IRQ vector
    call acknowledge        ; In case of hardware interrupt, inform PIC it was handled

    mov rax, [rsp + 8 * 15] ; IRQ vector
    cmp rax, 32             ; Switch task on timer interrupt
    je .do_switch
    cmp rax, 128            ; Switch task on software interrupt
    jne .do_not_switch

.do_switch:
    call get_current_process
    mov rbx, rax ; Save pointer to current process

    call get_next_process
    cmp rax, rbx ; Check if the current process is the same as the next process
    je .do_not_switch

    mov [rbx + 8], rsp ; Save current kernel stack pointer

    mov rsp, [rax]      ; Load next kernel stack top
    mov [ist.rsp0], rsp ; Update IST with next kernel stack top
    mov rsp, [rax + 8]  ; Load next kernel stack pointer

    mov rdi, rax ; Pointer to next process
    call swap_process

.do_not_switch:

    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rbp
    pop rdi
    pop rsi
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    add rsp, 8 * 2 ; Remove IRQ vector and (error) code from stack
    iretq

yield:
    int 0x80
    ret

isr:
    ISR_ENTRY 000, handler, 0, 0
    ISR_ENTRY 001, handler, 1, 0
    ISR_ENTRY 002, handler, 2, 0
    ISR_ENTRY 003, handler, 3, 0
    ISR_ENTRY 004, handler, 4, 0
    ISR_ENTRY 005, handler, 5, 0
    ISR_ENTRY 006, handler, 6, 0
    ISR_ENTRY 007, handler, 7, 0
    ISR_ENTRY 008, handler, 8
    ISR_ENTRY 010, handler, 10
    ISR_ENTRY 011, handler, 11
    ISR_ENTRY 012, handler, 12
    ISR_ENTRY 013, handler, 13
    ISR_ENTRY 014, handler, 14
    ISR_ENTRY 016, handler, 16, 0
    ISR_ENTRY 017, handler, 17
    ISR_ENTRY 018, handler, 18, 0
    ISR_ENTRY 019, handler, 19, 0
    ISR_ENTRY 021, handler, 21
    ISR_ENTRY 028, handler, 28, 0
    ISR_ENTRY 029, handler, 29
    ISR_ENTRY 030, handler, 30
    ISR_ENTRY 032, handler, 32, 0
    ISR_ENTRY 033, handler, 33, 1
    ISR_ENTRY 034, handler, 34, 2
    ISR_ENTRY 035, handler, 35, 3
    ISR_ENTRY 036, handler, 36, 4
    ISR_ENTRY 037, handler, 37, 5
    ISR_ENTRY 038, handler, 38, 6
    ISR_ENTRY 039, handler, 39, 7
    ISR_ENTRY 040, handler, 40, 8
    ISR_ENTRY 041, handler, 41, 9
    ISR_ENTRY 042, handler, 42, 10
    ISR_ENTRY 043, handler, 43, 11
    ISR_ENTRY 044, handler, 44, 12
    ISR_ENTRY 045, handler, 45, 13
    ISR_ENTRY 046, handler, 46, 14
    ISR_ENTRY 047, handler, 47, 15
    ISR_ENTRY 128, handler, 128, 0

section .data

align 0x1000
idt:
    IDT_ENTRY 000, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Divide by zero
    IDT_ENTRY 001, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Debug
    IDT_ENTRY 002, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Non-maskable
    IDT_ENTRY 003, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Breakpoint
    IDT_ENTRY 004, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Overflow
    IDT_ENTRY 005, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Bound range exceeded
    IDT_ENTRY 006, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Invalid opcode
    IDT_ENTRY 007, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Device not available
    IDT_ENTRY 008, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Double fault

    times 2 dq 0 ; Vector 9 is unsupported on x86_64 architectures

    IDT_ENTRY 010, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Invalid TSS
    IDT_ENTRY 011, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Segment not present
    IDT_ENTRY 012, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Stack segment fault
    IDT_ENTRY 013, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; General protection fault
    IDT_ENTRY 014, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Page fault

    times 2 dq 0 ; Vector 15 is reserved

    IDT_ENTRY 016, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; x87 floating point exception
    IDT_ENTRY 017, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Alignment check
    IDT_ENTRY 018, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Machine check
    IDT_ENTRY 019, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; SIMD floating point exception

    times 2 dq 0 ; Vector 20 is reserved

    IDT_ENTRY 021, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Control protection exception

    times 12 dq 0 ; Vectors 22 - 27 are reserved

    IDT_ENTRY 028, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Hypervisor injection exception
    IDT_ENTRY 029, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; VMM communication exception
    IDT_ENTRY 030, 0, IDT_PRESENT | IDT_RING(0) | IDT_TRAP, gdt.kernel_cs ; Security exception

    times 2 dq 0 ; Vector 31 is reserved

    IDT_ENTRY 032, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; PIT
    IDT_ENTRY 033, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; Keyboard
    IDT_ENTRY 034, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; PIC cascade
    IDT_ENTRY 035, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; COM2/COM4
    IDT_ENTRY 036, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; COM1/COM3
    IDT_ENTRY 037, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; LPT2
    IDT_ENTRY 038, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; FDC
    IDT_ENTRY 039, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; LPT1
    IDT_ENTRY 040, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; RTC
    IDT_ENTRY 041, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; Available
    IDT_ENTRY 042, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; Available
    IDT_ENTRY 043, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; Available
    IDT_ENTRY 044, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; Mouse
    IDT_ENTRY 045, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; FPU
    IDT_ENTRY 046, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; HDC1
    IDT_ENTRY 047, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; HDC2

    times 160 dq 0 ; Vectors 48 - 127 are unused

    IDT_ENTRY 128, 0, IDT_PRESENT | IDT_RING(0) | IDT_INT, gdt.kernel_cs ; Syscall
.pointer:
    dw $ - idt - 1
    dq idt

.count: equ (idt.pointer - idt) / 16

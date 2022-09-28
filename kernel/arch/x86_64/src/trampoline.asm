bits 64

extern bss_start, bss_size, ctor_start, ctor_count
extern init_gdt, gdt.kernel_cs, gdt.kernel_ds
extern KernelInit, KernelMain

global trampoline

section .text

trampoline:
    ; UEFI provides a GDT but we want our own
    call init_gdt

    ; Update data segments
    mov ax, gdt.kernel_ds
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Update code segment
    push gdt.kernel_cs
    lea rax, [start]
    push rax
    retfq

start:
    ; Set up new stack
    mov rsp, stack_top

    ; Zero-out BSS section
    xor rax, rax
    mov rcx, bss_size
    mov rdi, bss_start
    rep stosb

    ; Set up critical services before calling global constructors
    call KernelInit

call_global_ctors:
    mov rbx, ctor_start
    mov rcx, ctor_count
    cmp rcx, 0
    je .done
.next:
    push rcx
    call [rbx]
    pop rcx
    add rbx, 8
    loop .next
.done:

    jmp KernelMain

section .bss

resb 0x1000
stack_top:

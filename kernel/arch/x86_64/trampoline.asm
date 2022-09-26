extern _bss_start, _bss_size, _ctor_start, _ctor_count, _dtor_end, _dtor_count
extern _init_gdt, _gdt.pl0_cs_dsc, _gdt.pl0_ds_dsc
extern KernelInit, KernelMain

global _trampoline

bits 64

section .text

_trampoline:
    ; UEFI provides a GDT but we want our own
    call _init_gdt

    ; Update data segments
    mov ax, _gdt.pl0_ds_dsc
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Update code segment
    push _gdt.pl0_cs_dsc
    lea rax, [start]
    push rax
    retfq

start:
    ; Set up new stack
    mov rsp, stack_top

    ; Zero-out BSS section
    xor rax, rax
    mov rcx, _bss_size
    mov rdi, _bss_start
    rep stosb

    ; Set up critical services before calling global constructors
    call KernelInit

call_global_ctors:
    mov rbx, _ctor_start
    mov rcx, _ctor_count
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

extern _bss_start, _bss_size, _ctor_start, _ctor_count, _dtor_end, _dtor_count
extern _ktop, _kpages, _mmap, _gfxo
extern KernelInit, KernelMain

bits 64

section .text

global _trampoline
_trampoline:
    ; UEFI provides a GDT but we want our own
    lgdt [gdt.ptr]

    ; Update data segments
    mov ax, gdt.kData
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Update code segment
    push gdt.kCode
    lea rax, [start]
    push rax
    retfq

start:
    ; Set up new stack
    mov rsp, kStackTop

    ; Zero-out BSS section
    xor rax, rax
    mov rcx, _bss_size
    mov rdi, _bss_start
    rep stosb

    ; Pass parameters to kernel
    mov rdi, [_ktop]   ; Kernel top physical address
    mov rsi, [_kpages] ; Kernel page count
    mov rdx, [_mmap]   ; Memory map pointer
    mov rcx, [_gfxo]   ; Graphics output pointer

    ; Set up critical services before calling global constructors
    call KernelInit

    ; Call global constructors
call_global_ctors:
    mov rbx, _ctor_start
    mov rcx, _ctor_count
    cmp rcx, 0
    je .done
.next:
    call [rbx]
    add rbx, 8
    loop .next
.done:

    call KernelMain

    ; Call global destructors (this isn't really necessary)
call_global_dtors:
    mov rbx, _dtor_end
    mov rcx, _dtor_count
    cmp rcx, 0
    je .done
.next:
    sub rbx, 8
    call [rbx]
    loop .next
.done:

    cli
    hlt
    jmp $

section .rodata

align 8
gdt:
.null:  dq 0
.kCode: equ $ - gdt
        dq 0x00209A0000000000 ; Kernel 64-bit code segment RWX
.kData: equ $ - gdt
        dq 0x0000920000000000 ; Kernel 64-bit data segment RW
.uCode: equ $ - gdt
        dq 0x0020FA0000000000 ; User 64-bit code segment RWX
.uData: equ $ - gdt
        dq 0x0000F20000000000 ; User 64-bit data segment RW
.ptr:   dw $ - gdt - 1
        dq gdt

section .bss

resb 0x8000
kStackTop:

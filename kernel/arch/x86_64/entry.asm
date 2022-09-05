extern _bss_start, _bss_size, _ctor_start, _ctor_count, _dtor_end, _dtor_count
extern KernelInit, KernelMain

bits 64

section .text

global _entry
_entry:
    cli ; No interrupts
    cld ; Index registers increment

    mov [mmap], rdi ; Save memory map pointer
    mov [acpi], rsi ; Save ACPI table pointer

    lgdt [gdt.ptr] ; Load GDT register

    ; Reload GDT data descriptor
    mov ax, gdt.kData
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Reload GDT code descriptor
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
    mov rdi, [mmap] ; Load memory map pointer
    mov rsi, [acpi] ; Load ACPI table pointer

    call KernelInit

    ; Call global constructors
call_global_ctors:
    mov rbx, _ctor_start
    mov rcx, _ctor_count
    cmp rcx, 0
    je .done
.loop:
    call [rbx]
    add rbx, 8
    loop .loop
.done:

    call KernelMain

    ; Call global destructors
call_global_dtors:
    mov rbx, _dtor_end
    mov rcx, _dtor_count
    cmp rcx, 0
    je .done
.loop:
    sub rbx, 8
    call [rbx]
    loop .loop
.done:

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

section .data

acpi: dq 0
mmap: dq 0

section .bss

resb 0x2000
kStackTop:

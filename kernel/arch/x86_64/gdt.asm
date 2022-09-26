global _init_gdt, _gdt.pl0_cs_dsc, _gdt.pl0_ds_dsc

bits 64

section .text

_init_gdt:
    ; Set TSS descriptor base to point to the Interrupt Stack Table
    lea rax, ist
    mov [_gdt.tss + 2], ax  ; Base 0-15
    shr rax, 16
    mov [_gdt.tss + 4], al  ; Base 16-23
    mov [_gdt.tss + 7], ah  ; Base 24-31
    shr rax, 16
    mov [_gdt.tss + 8], eax ; Base 32-63

    ; Load GDT
    lgdt [_gdt.pointer]

    ; Load TSS
    mov ax, _gdt.tss_dsc
    ltr ax
    ret

section .data

align 8
_gdt:
.null:
    dq 0
.pl0_cs_dsc: equ $ - _gdt
.pl0_cs:
    dq 0x00209A0000000000 ; Kernel 64-bit code segment RWX
.pl0_ds_dsc: equ $ - _gdt
.pl0_ds:
    dq 0x0000920000000000 ; Kernel 64-bit data segment RW
.pl3_cs_dsc: equ $ - _gdt
.pl3_cs:
    dq 0x0020FA0000000000 ; User 64-bit code segment RWX
.pl3_ds_dsc: equ $ - _gdt
.pl3_ds:
    dq 0x0000F20000000000 ; User 64-bit data segment RW
.tss_dsc:    equ $ - _gdt
.tss:
    dq 0x0040890000000000 | ist.size
    dq 0x0000000000000000
.pointer:
    dw $ - _gdt - 1
    dq _gdt

section .bss

align 8
ist:
    resb 0x68
.size: equ $ - ist

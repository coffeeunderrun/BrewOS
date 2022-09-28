bits 64

global init_gdt, gdt.kernel_cs, gdt.kernel_ds

section .text

init_gdt:
    ; Set TSS descriptor base to point to the Interrupt Stack Table   
    lea rax, ist
    lea rdi, gdt + gdt.tss + 2 ; Skip limit 0-15
    stosw                      ; Base 0-15
    shr rax, 16
    stosb                      ; Base 16-23
    add rdi, 2                 ; Skip access, flags, and limit 16-19
    shr rax, 8
    stosb                      ; Base 24-31
    shr rax, 8
    stosd                      ; Base 32-63

    ; Load GDT
    lgdt [gdt.pointer]

    ; Load TSS
    mov ax, gdt.tss
    ltr ax
    ret

section .data

align 8
gdt:
.null:      dq 0
.kernel_cs: equ $ - gdt
            dq 0x00209A0000000000 ; Kernel 64-bit code segment RWX
.kernel_ds: equ $ - gdt
            dq 0x0000920000000000 ; Kernel 64-bit data segment RW
.user_cs:   equ $ - gdt
            dq 0x0020FA0000000000 ; User 64-bit code segment RWX
.user_ds:   equ $ - gdt
            dq 0x0000F20000000000 ; User 64-bit data segment RW
.tss:       equ $ - gdt
            dq 0x0040890000000000 | ist.size
            dq 0x0000000000000000
.pointer:   dw $ - gdt - 1
            dq gdt

section .bss

align 8
ist:   resb 0x68
.size: equ $ - ist

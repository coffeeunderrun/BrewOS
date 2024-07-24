bits 64

global initialize_gdt, gdt.kernel_cs, ist.rsp0

%define GDT_LONG    1 << 53 ; Long mode (64-bit)
%define GDT_PRESENT 1 << 47 ; Present
%define GDT_RING(n) n << 45 ; Privilege level (ring-n)
%define GDT_DATA    2 << 43 ; Data segment
%define GDT_CODE    3 << 43 ; Code segment
%define GDT_RW      1 << 41 ; Readable code segment, writeable data segment
%define GDT_ACCESS  1 << 40 ; Accessed
%define GDT_LDT     2 << 40 ; Local descriptor table
%define GDT_TSS     9 << 40 ; Task state segment
%define GDT_BUSY    1 << 41 ; Task state segment busy

section .text

initialize_gdt:
    call update_tss_entry

    lgdt [gdt.pointer]

    mov ax, gdt.tss
    ltr ax

    mov ax, gdt.kernel_ds
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rax, gdt.kernel_cs
    xchg rax, [rsp] ; Swap caller RIP on stack with kernel code segment
    push rax        ; Push caller RIP to top of stack
    retfq           ; Far return to kernel_CS:caller_RIP

update_tss_entry:
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
    ret

section .data

align 8
gdt:
.null:
    dq 0

.kernel_cs: equ $ - gdt
    dq 0 | GDT_PRESENT | GDT_LONG | GDT_RING(0) | GDT_CODE | GDT_ACCESS | GDT_RW

.kernel_ds: equ $ - gdt
    dq 0 | GDT_PRESENT | GDT_LONG | GDT_RING(0) | GDT_DATA | GDT_ACCESS | GDT_RW

.user_cs: equ $ - gdt
    dq 0 | GDT_PRESENT | GDT_LONG | GDT_RING(3) | GDT_CODE | GDT_ACCESS | GDT_RW

.user_ds: equ $ - gdt
    dq 0 | GDT_PRESENT | GDT_LONG | GDT_RING(3) | GDT_DATA | GDT_ACCESS | GDT_RW

.tss: equ $ - gdt
    dq 0 | GDT_PRESENT | GDT_LONG | GDT_RING(0) | GDT_TSS | ist.size
    dq 0

.pointer:
    dw $ - gdt - 1
    dq gdt

section .bss

align 8
ist:   resd 1 ; Reserved
.rsp0: resq 1
.rsp1: resq 1
.rsp2: resq 1
       resq 1 ; Reserved
.ist1: resq 1
.ist2: resq 1
.ist3: resq 1
.ist4: resq 1
.ist5: resq 1
.ist6: resq 1
.ist7: resq 1
       resq 1 ; Reserved
       resw 1 ; Reserved
.iopb: resw 1
.size: equ $ - ist

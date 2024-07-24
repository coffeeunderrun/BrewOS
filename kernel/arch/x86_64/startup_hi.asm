bits 64

global startup_hi, stack_top

extern bss_start, bss_size
extern initialize, initialize_gdt, initialize_idt, initialize_pit, initialize_graphics
extern create_idle_process, create_kernel_process

section .text

; Expects RBX to contain the pointer to kernel data from boot loader

startup_hi:
    xor rax, rax
    mov rcx, bss_size
    mov rdi, bss_start ; Zero-out BSS section
    rep stosb

    mov rsp, stack_top ; Set up new stack

    push rbx ; Save pointer to kernel data

    call initialize_gdt
    call initialize_idt
    call initialize_pit

    pop rdi ; Restore pointer to kernel data

    mov rdi, [rdi + 8 * 3] ; Pointer to graphics output protocol
    call initialize_graphics

    lea rdi, process_name.idle
    call create_idle_process

    lea rdi, process_name.initialize
    lea rsi, initialize
    call create_kernel_process

    sti

idle_loop:
    hlt
    jmp idle_loop

section .data

process_name:
.idle:
    db "Idle", 0
.initialize:
    db "Initialize", 0

section .bss

align 16
stack_bottom:
    resb 0x1000
stack_top:

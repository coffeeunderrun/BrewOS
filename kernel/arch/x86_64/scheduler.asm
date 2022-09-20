bits 64

struc Thread
    .rsp: resq 1
    .cr3: resq 1
endstruc

section .text

global _switch
_switch:
    ; Save "from" thread's registers
    pushfq
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15
    mov [rdi + Thread.rsp], rsp

    ; Load "to" thread's registers
    mov rsp, [rsi + Thread.rsp]
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    popfq

    ret

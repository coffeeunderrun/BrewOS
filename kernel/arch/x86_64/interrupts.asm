extern InterruptHandler, _gdt.pl0_cs_dsc

global _init_interrupts

bits 64

PIC1_CTRL_PORT equ 0x20
PIC1_DATA_PORT equ 0x21
PIC2_CTRL_PORT equ 0xA0
PIC2_DATA_PORT equ 0xA1
PIC_END_IRQ    equ 0x20
ICW1           equ 0x11
ICW2_MASTER    equ 0x20
ICW2_SLAVE     equ 0x28
ICW3_MASTER    equ 0x4
ICW3_SLAVE     equ 0x2
ICW4           equ 0x1

%macro IDT_INTERRUPT 1-2 0
    mov rax, isr%1           ; Address of ISR
    mov cx, _gdt.pl0_cs_dsc  ; Segment selector
    mov dh, 0x8E             ; Flags
    mov dl, %2               ; IST offset
    mov rdi, idt + (%1 * 16) ; Point to entry in IDT
    call load_idt_entry
%endmacro

%macro IDT_EXCEPTION 1-2 0
    mov rax, isr%1           ; Address of ISR
    mov cx, _gdt.pl0_cs_dsc  ; Segment selector
    mov dh, 0x8F             ; Flags
    mov dl, %2               ; IST offset
    mov rdi, idt + (%1 * 16) ; Point to entry in IDT
    call load_idt_entry
%endmacro

%macro PUSH_ALL 0
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
%endmacro

%macro POP_ALL 0
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
%endmacro

%macro ISR_ERROR 1
isr%1:
    pop qword [error]     ; Save error code and remove from stack
    PUSH_ALL              ; Push all registers to stack
    mov rdi, %1           ; Vector
    mov rsi, [error]      ; Error code
    mov rdx, rsp          ; Pointer to register structure
    call InterruptHandler ; Pass parameters to interrupt handler
    POP_ALL               ; Restore registers from stack
    iretq
%endmacro

%macro ISR_NO_ERROR 1
isr%1:
    PUSH_ALL              ; Push all registers to stack
    mov rdi, %1           ; Vector
    xor rsi, rsi          ; Zero (no error)
    mov rdx, rsp          ; Pointer to register structure
    call InterruptHandler ; Pass parameters to interrupt handler
    POP_ALL               ; Restore registers from stack
    iretq
%endmacro

%macro ISR_HARDWARE 1
isr%1:
    PUSH_ALL              ; Push all registers to stack
    mov rdi, %1           ; Vector
    xor rsi, rsi          ; Zero (no error)
    mov rdx, rsp          ; Pointer to register structure
    call InterruptHandler ; Pass parameters to interrupt handler
    mov rdi, %1           ; Vector
    call ack_interrupt    ; Tell PIC interrupt was acknowledged
    POP_ALL               ; Restore registers from stack
    iretq
%endmacro

section .text

_init_interrupts:
    ; Initialize programmable interrupt controllers
    mov al, ICW1
    out PIC1_CTRL_PORT, al
    out PIC2_CTRL_PORT, al

    ; Remap PICs as first 32 interrupts are reserved
    mov al, ICW2_MASTER
    out PIC1_DATA_PORT, al
    mov al, ICW2_SLAVE
    out PIC2_DATA_PORT, al

    ; Cascading
    mov al, ICW3_MASTER
    out PIC1_DATA_PORT, al
    mov al, ICW3_SLAVE
    out PIC2_DATA_PORT, al

    ; Environment info
    mov al, ICW4
    out PIC1_DATA_PORT, al
    out PIC2_DATA_PORT, al

    ; Mask interrupts
    xor al, al
    out PIC1_DATA_PORT, al
    out PIC2_DATA_PORT, al

    ; Zero out IDT
    xor rax, rax
    mov rcx, 0x200
    mov rdi, qword idt
    rep stosq

    ; Load entries into IDT
    IDT_EXCEPTION 0 ; Divide by zero error
    IDT_EXCEPTION 1 ; Debug
    IDT_EXCEPTION 2 ; Non-maskable interrupt
    IDT_EXCEPTION 3 ; Breakpoint
    IDT_EXCEPTION 4 ; Overflow
    IDT_EXCEPTION 5 ; Bound range exceeded
    IDT_EXCEPTION 6 ; Invalid opcode
    IDT_EXCEPTION 7 ; Device not available
    IDT_EXCEPTION 8 ; Double fault
    
    ; Vector 9 is unsupported on x86_64 architectures

    IDT_EXCEPTION 10 ; Invalid TSS
    IDT_EXCEPTION 11 ; Segment not present
    IDT_EXCEPTION 12 ; Stack segment fault
    IDT_EXCEPTION 13 ; General protection fault
    IDT_EXCEPTION 14 ; Page fault

    ; Vector 15 is reserved

    IDT_EXCEPTION 16 ; x87 floating point exception
    IDT_EXCEPTION 17 ; Alignment check
    IDT_EXCEPTION 18 ; Machine check
    IDT_EXCEPTION 19 ; SIMD floating point exception

    ; Vector 20 is reserved

    IDT_EXCEPTION 21 ; Control protection exception

    ; Vectors 22 - 27 are reserved

    IDT_EXCEPTION 28 ; Hypervisor injection exception
    IDT_EXCEPTION 29 ; VMM communication exception
    IDT_EXCEPTION 30 ; Security exception

    ; Vector 31 is reserved

    IDT_INTERRUPT 32 ; PIT
    IDT_INTERRUPT 33 ; Keyboard
    IDT_INTERRUPT 34 ; PIC cascade
    IDT_INTERRUPT 35 ; COM2/COM4
    IDT_INTERRUPT 36 ; COM1/COM3
    IDT_INTERRUPT 37 ; LPT2
    IDT_INTERRUPT 38 ; Floppy disk controller
    IDT_INTERRUPT 39 ; LPT1
    IDT_INTERRUPT 40 ; RTC
    IDT_INTERRUPT 41 ; Available
    IDT_INTERRUPT 42 ; Available
    IDT_INTERRUPT 43 ; Available
    IDT_INTERRUPT 44 ; Mouse
    IDT_INTERRUPT 45 ; FPU
    IDT_INTERRUPT 46 ; Primary hard disk controller
    IDT_INTERRUPT 47 ; Secondary hard disk controller

    ; Vectors 48 - 255 are unused

    mov rax, qword idt_ptr
    lidt [rax]
    sti
    ret

; Add descriptor entry to IDT
; Address of ISR in RAX
; Segment selector in CX
; IST offset in DL
; Flags in DH
load_idt_entry:
    stosw        ; ISR offset low 16 bits
    mov ax, cx   ; Segment selector
    stosw
    mov ax, dx   ; IST offset and flags
    stosw
    shr rax, 16  ; ISR offset mid 16 bits
    stosw
    shr rax, 16  ; ISR offset high 32 bits
    stosd
    xor rax, rax ; reserved
    stosd
    ret

; Tell the PIC that the interrupt has been handled
; Interrupt vector to be cleared in AL
; Vectors 32-39, originally 0-7, are PIC1
; Vectors 40-47, originally 8-15, are PIC2
ack_interrupt:
    cmp al, 32 ; Ensure vector is >= 32
    jb .done
    cmp al, 47 ; Ensure vector is <= 47
    ja .done
    cmp al, 40 ; Which PIC should be informed?
    mov al, PIC_END_IRQ
    jge .2     ; When vector is >= 40 select PIC2
.1: mov dx, PIC1_CTRL_PORT
    jmp .3     ; Vector is < 40 so PIC1 is selected
.2: mov dx, PIC2_CTRL_PORT
.3: out dx, al
.done:
    ret

; Software interrupts
ISR_NO_ERROR 0  ; Divide by zero error
ISR_NO_ERROR 1  ; Debug
ISR_NO_ERROR 2  ; Non-maskable interrupt
ISR_NO_ERROR 3  ; Breakpoint
ISR_NO_ERROR 4  ; Overflow
ISR_NO_ERROR 5  ; Bound range exceeded
ISR_NO_ERROR 6  ; Invalid opcode
ISR_NO_ERROR 7  ; Device not available
ISR_ERROR    8  ; Double fault
ISR_ERROR    10 ; Invalid TSS
ISR_ERROR    11 ; Segment not present
ISR_ERROR    12 ; Stack segment fault
ISR_ERROR    13 ; General protection fault
ISR_ERROR    14 ; Page fault
ISR_NO_ERROR 16 ; x87 floating point exception
ISR_ERROR    17 ; Alignment check
ISR_NO_ERROR 18 ; Machine check
ISR_NO_ERROR 19 ; SIMD floating point exception
ISR_ERROR    21 ; Control protection exception
ISR_NO_ERROR 28 ; Hypervisor injection exception
ISR_ERROR    29 ; VMM communication exception
ISR_ERROR    30 ; Security exception

; Hardware interrupts
ISR_HARDWARE 32 ; PIT
ISR_HARDWARE 33 ; Keyboard
ISR_HARDWARE 34 ; PIC cascade
ISR_HARDWARE 35 ; COM2/COM4
ISR_HARDWARE 36 ; COM1/COM3
ISR_HARDWARE 37 ; LPT2
ISR_HARDWARE 38 ; Floppy disk controller
ISR_HARDWARE 39 ; LPT1
ISR_HARDWARE 40 ; RTC
ISR_HARDWARE 41 ; Available
ISR_HARDWARE 42 ; Available
ISR_HARDWARE 43 ; Available
ISR_HARDWARE 44 ; Mouse
ISR_HARDWARE 45 ; FPU
ISR_HARDWARE 46 ; Primary hard disk controller
ISR_HARDWARE 47 ; Secondary hard disk controller

section .data

align 8
idt_ptr:
    dw idt.size ; Limit
    dq idt      ; Offset

error:
    dq 0

section .bss

align 0x1000
idt:
    resb 0x1000
.size: equ $ - idt - 1

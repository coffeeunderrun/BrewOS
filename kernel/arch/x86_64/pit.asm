bits 64

global initialize_pit

%define PIT_CH0_PORT 0x40
%define PIT_CH1_PORT 0x41
%define PIT_CH2_PORT 0x42
%define PIT_CMD_PORT 0x43

%define PIT_FRQ 1193182
%define PIT_DIV 100

section .text

initialize_pit:
    mov al, 0x34
    out PIT_CMD_PORT, al

    mov ax, PIT_FRQ / PIT_DIV
    out PIT_CH0_PORT, al

    shr ax, 8
    out PIT_CH0_PORT, al
    ret

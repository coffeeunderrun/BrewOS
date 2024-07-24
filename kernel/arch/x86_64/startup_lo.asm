bits 64

global startup_lo

extern startup_hi, rx_page_count, ro_page_count, rw_page_count

pml4 equ 0x8000 ; PML4
pml3 equ 0x9000 ; PML3
pml2 equ 0xA000 ; PML2
pml1 equ 0xB000 ; PML1 (first of multiple tables; count based on kernel pages)

%define PTE_PRESENT 0x01  ; Present
%define PTE_RW      0x02  ; Read/Write
%define PTE_USER    0x04  ; User
%define PTE_WTCACHE 0x08  ; Write-through cache
%define PTE_NOCACHE 0x10  ; No cache
%define PTE_HUGE    0x80  ; PML3 = 1 GiB; PML2 = 2 Mib
%define PTE_GLOBAL  0x100 ; Global
%define PTE_NOEXEC  0x3F  ; No execute bit 63

%define CR4_GLOBAL  0x80 ; Enable global pages

struc KernelData
    .image_base: resq 1 ; Pointer to first kernel page
    .page_count: resq 1 ; Number of kernel pages
endstruc

section .text

; Using System V AMD64 ABI calling convention
; https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI
; Check parameters passed from boot loader before trashing registers

startup_lo:
    cli
    cld

    mov rbx, rdi ; Save kernel data pointer from boot loader

clear_page_tables:
    xor rax, rax
    mov rcx, [rbx + KernelData.page_count]
    add rcx, 0x1FF ; Add 511 (page table entry count - 1)
    shr rcx, 9     ; Divide by 512 to get page table count
    add rcx, 3     ; Add 3 for PML4, PML3, and PML2 page tables
    shl rcx, 9     ; Multiply by 512 (512 entries per table; 8 bytes per entry)
    mov rdi, pml4
    rep stosq

; TODO: map frame buffer and memory map directly rather than first 3 GiB
load_pml4_table:
    mov rax, pml3
    or  rax, PTE_PRESENT | PTE_RW
    mov [pml4], rax           ; Identity map first 512 GiB
    mov [pml4 + 8 * 511], rax ; Map last 512 GiB to first 512 GiB

    mov rax, pml4
    or  rax, PTE_PRESENT | PTE_RW
    bts rax, PTE_NOEXEC
    mov [pml4 + 8 * 510], rax ; Recursively map entry 510 of PML4 to itself


load_pml3_table:
    mov rax, PTE_PRESENT | PTE_RW | PTE_HUGE
    mov rcx, 3 ; Identity map first 3 GiB
    mov rdi, pml3
.loop:
    stosq
    add rax, 0x40000000 ; Next 1 GiB
    dec rcx
    jnz .loop

    mov rax, pml2
    or  rax, PTE_PRESENT | PTE_RW
    mov [pml3 + 8 * 510], rax ; Map second to last 1 GiB (-2 GiB)

load_pml2_table:
    mov rax, pml1
    or  rax, PTE_PRESENT | PTE_RW
    mov [pml2], rax ; Map first 2 MiB of -2 GiB

load_pml1_tables:
    mov rax, [rbx + KernelData.image_base]
    or  rax, PTE_PRESENT | PTE_GLOBAL
    mov rdi, pml1

    mov rcx, rx_page_count ; Number of read-only, executable code pages
.rx_loop:
    stosq
    add rax, 0x1000 ; Next 4 KiB
    dec rcx
    jnz .rx_loop

    bts rax, PTE_NOEXEC
    mov rcx, ro_page_count ; Number of read-only data pages
.ro_loop:
    stosq
    add rax, 0x1000 ; Next 4 KiB
    dec rcx
    jnz .ro_loop

    or  rax, PTE_RW
    mov rcx, rw_page_count ; Number of readable, writeable data pages
.rw_loop:
    stosq
    add rax, 0x1000 ; Next 4 KiB
    dec rcx
    jnz .rw_loop

trampoline:
    mov rax, pml4
    mov cr3, rax

    mov rax, cr4
    or  rax, CR4_GLOBAL
    mov cr4, rax

    lea rax, startup_hi ; Jump to higher half
    jmp rax

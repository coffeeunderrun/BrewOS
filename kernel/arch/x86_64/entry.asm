extern _trampoline

bits 64

p4 equ 0x8000 ; Temporary PML4
p3 equ 0x9000 ; Temporary PDP
p2 equ 0xA000 ; Temporary PDI
p1 equ 0xB000 ; Temporary PTI (first of multiple tables; count based on kernel pages)

section .text

global _entry
_entry:
    cli ; No interrupts
    cld ; Index registers increment

    ; Using System V AMD64 ABI calling convention
    ; https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI
    ; Check parameters passed from the UEFI loader before trashing registers

    mov [_ktop], rdi   ; Save kernel top physical adress
    mov [_kpages], rsi ; Save kernel page count
    mov [_mmap], rdx   ; Save memory map pointer

    ; Zero out level 4, 3, and 2 page tables
    xor rax, rax
    mov rcx, 512 * 3
    mov rdi, p4
    rep stosq

    ; Zero out level 1 page tables
    mov rcx, [_kpages]
    imul rcx, 512
    mov rdi, p1
    rep stosq

    ; Only map the pages where the UEFI loader physically loaded the kernel
    ; These page tables are temporary and will be trashed by the kernel
load_page_tables:
    ; PML4 table
    mov rax, p3
    or rax, 0x3             ; Present and writable
    mov [p4], rax           ; Identity map first 512 GiB
    mov [p4 + 511 * 8], rax ; Map last 512 GiB to first 512 GiB

    ; Page directory pointer table
    mov rax, 0x83           ; Present, writable, and 1 GiB pages
    mov [p3], rax           ; Identity map first 1 GiB
    mov rax, p2
    or rax, 0x3             ; Present, writable, and 2 MiB pages
    mov [p3 + 510 * 8], rax ; Map second to last 1 GiB to PDI table

    ; Page directory index (-2 GiB)
    mov rax, p1
    or rax, 0x3 ; Present, writable, and 4 KiB pages
    mov [p2], rax

    ; Page table index (map physical kernel pages to start of -2 GiB)
    mov rax, [_ktop]   ; Start at kernel physical top
    or rax, 0x3        ; Present and writable
    mov rcx, [_kpages] ; Loop through each kernel page
    mov rdi, p1
.next:
    stosq
    add rax, 0x1000 ; Move to next kernel physical page
    loop .next

    ; Point CR3 at PML4 table
    mov rax, p4
    mov cr3, rax

    ; Jump to higher-half virtual address space
    lea rax, _trampoline
    jmp rax

section .data

global _mmap
_mmap: dq 0

global _ktop
_ktop: dq 0

global _kpages
_kpages: dq 0

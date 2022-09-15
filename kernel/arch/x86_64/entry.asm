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

    ; Save arguments passed from UEFI
    mov [_kernel_start], rdi    ; Kernel top physical adress
    mov [_kernel_pages], rsi    ; Kernel page count
    mov [_memory_map], rdx      ; Memory map pointer
    mov [_graphics_output], rcx ; Graphics output pointer
    mov [_font], r8             ; Font

    ; Zero out level 4, 3, and 2 page tables
    xor rax, rax
    mov rcx, 512 * 3
    mov rdi, p4
    rep stosq

    ; Zero out level 1 page tables
    mov rcx, [_kernel_pages]
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
    add rax, 0x40000000
    ; TODO: map framebuffer rather than first 3 GiB
    mov [p3 + 1 * 8], rax   ; Identity map second 1 GiB
    add rax, 0x40000000
    mov [p3 + 2 * 8], rax   ; Identity map third 1 GiB
    mov rax, p2
    or rax, 0x3             ; Present, writable, and 2 MiB pages
    mov [p3 + 510 * 8], rax ; Map second to last 1 GiB to PDI table

    ; Page directory index (-2 GiB)
    mov rax, p1
    or rax, 0x3 ; Present, writable, and 4 KiB pages
    mov [p2], rax

    ; TODO: this should be handled by kernel VMM once implemented
    ; Map kernel heap
    mov rax, 0x83   ; Present, writable, and 2 MiB pages
    mov rcx, 4      ; 4 * 2 MiB = 8 MiB
    mov rdi, p2 + 8 ; Start at second 2 MiB
.next_p2:
    stosq
    add rax, 200000 ; Next 2 MiB
    loop .next_p2

    ; Page table index (map physical kernel pages to start of -2 GiB)
    mov rax, [_kernel_start] ; Start at kernel physical top
    or rax, 0x3              ; Present and writable
    mov rcx, [_kernel_pages] ; Loop through each kernel page
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

global _kernel_start
_kernel_start:
    dq 0

global _kernel_pages
_kernel_pages:
    dq 0

global _memory_map
_memory_map:
    dq 0

global _graphics_output
_graphics_output:
    dq 0

global _font
_font:
    dq 0

bits 64

p1 equ 0x8000 ; Temporary PTI (first of multiple tables; count based on kernel pages)

struc Kernel
    .entry: resq 1
    .top:   resq 1
    .pages: resq 1
endstruc

section .text

global _entry
_entry:
    cli ; No interrupts
    cld ; Index registers increment

    ; Using System V AMD64 ABI calling convention
    ; https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI
    ; Check parameters passed from the UEFI loader before trashing registers

    mov [mmap], rsi ; Save memory map pointer
    mov [acpi], rdx ; Save ACPI table pointer

    ; Save kernel info
    mov rsi, rdi
    mov rdi, kernel
    mov rcx, Kernel_size
    rep movsb

    ; Zero out page tables
    xor rax, rax
    mov rcx, 512 * 3
    mov rdi, p4
    rep stosq

    mov rcx, [kernel.pages]
    imul rcx, 512
    mov rdi, p1
    rep stosq

    ; Only map the pages where the UEFI loader physically loaded the kernel
    ; These page tables are temporary and will be trashed by the kernel

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
    or rax, 0x3             ; Present, writable, and 4 KiB pages
    mov [p2], rax

    ; Page table index (map physical kernel pages to start of -2 GiB)
    mov rax, [kernel.top]
    or rax, 0x3             ; Present and writable
    mov rcx, [kernel.pages]
    mov rdi, p1
.loop:
    stosq
    add rax, 0x1000
    loop .loop

    ; Point CR3 at the PML4
    mov rdx, p4
    mov cr3, rdx

    ; Pass parameters to kernel
    mov rdi, [mmap] ; Load memory map pointer
    mov rsi, [acpi] ; Load ACPI table pointer

    ; Jump to kernel entry in higher-half virtual address space
    jmp [kernel.entry]

section .data

acpi: dq 0
mmap: dq 0

kernel:
istruc Kernel
    at Kernel.entry, .entry dq 0
    at Kernel.top,   .top   dq 0
    at Kernel.pages, .pages dq 0
iend

section .bss
p4: resb 0x1000 ; Temporary PML4
p3: resb 0x1000 ; Temporary PDP
p2: resb 0x1000 ; Temporary PDI

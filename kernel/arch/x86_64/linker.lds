OUTPUT_FORMAT(elf64-x86-64)
ENTRY(entry)

VIRTUAL_BASE_ADDRESS = 0xFFFFFFFF80000000;

SECTIONS
{
    /* Lower-half */
    .entry.text ALIGN(4K) :
    {
        *entry*(.text*)
    }

    .entry.data ALIGN(4K) :
    {
        *entry*(.data*)
    }

    /* Higher-half */
    . += VIRTUAL_BASE_ADDRESS;

    .text ALIGN(4K) :
    {
        *(.text*)
    }

    .ctors ALIGN(4K) :
    {
        ctor_start = .;
        *(SORT(.ctors*))
    }
    ctor_count = SIZEOF(.ctors) / 8;

    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }

    .data ALIGN(4K) :
    {
        *(.data*)
    }

    .bss ALIGN(4K) :
    {
        bss_start = .;
        *(.bss*)
        *(COMMON)
    }
    bss_size = SIZEOF(.bss);

    /* Align for 2 MiB pages */
    . = ALIGN(2M);
    heap_start = .;
    . += 8M;
    heap_end = .;

    /DISCARD/ :
    {
        *(.comment)
    }
}

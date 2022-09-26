OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_entry)

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
        _ctor_start = .;
        *(SORT(.ctors*))
    }
    _ctor_count = SIZEOF(.ctors) / 8;

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
        _bss_start = .;
        *(.bss*)
        *(COMMON)
    }
    _bss_size = SIZEOF(.bss);

    /* Align for 2 MiB pages */
    . = ALIGN(2M);
    _heap_start = .;
    . += 8M;
    _heap_end = .;

    /DISCARD/ :
    {
        *(.comment)
    }
}

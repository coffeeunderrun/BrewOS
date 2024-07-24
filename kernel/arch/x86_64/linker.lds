OUTPUT_FORMAT(elf64-x86-64)
ENTRY(startup_lo)

VIRTUAL_BASE_ADDRESS = 0xFFFFFFFF80000000;

SECTIONS
{
    /* Lower-half */
    .startup_lo.text ALIGN(4K) :
    {
        *startup_lo*(.text*)
    }

    /* Higher-half */
    . += VIRTUAL_BASE_ADDRESS;

    .text ALIGN(4K) :
    {
        *(.text*)
    }

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

    rx_page_count = ((SIZEOF(.text) + 0xFFF) / 0x1000) + ((SIZEOF(.startup_lo.text) + 0xFFF) / 0x1000);
    ro_page_count = ((SIZEOF(.rodata) + 0xFFF) / 0x1000);
    rw_page_count = ((SIZEOF(.data) + 0xFFF) / 0x1000) + ((SIZEOF(.bss) + 0xFFF) / 0x1000);

    /DISCARD/ :
    {
        *(.comment)
    }
}

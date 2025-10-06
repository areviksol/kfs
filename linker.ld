/* linker.ld - Linker script for the kernel */

ENTRY(_start)

SECTIONS
{
    /* Kernel starts at 1 MiB in physical memory */
    . = 1M;

    /* Multiboot header section */
    .multiboot :
    {
        *(.multiboot)
    }

    /* Text section for code */
    .text ALIGN(4K) : AT(ADDR(.text))
    {
        *(.text)
    }

    /* Read-only data */
    .rodata ALIGN(4K) : AT(ADDR(.rodata))
    {
        *(.rodata)
    }

    /* Read-write data (initialized) */
    .data ALIGN(4K) : AT(ADDR(.data))
    {
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    .bss ALIGN(4K) : AT(ADDR(.bss))
    {
        *(COMMON)
        *(.bss)
    }
}

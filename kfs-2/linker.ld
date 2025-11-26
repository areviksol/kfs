OUTPUT_FORMAT("elf32-i386")
ENTRY(start)

SECTIONS
{
    /* Place the GDT at physical 0x00000800 (mandatory for the task) */
    . = 0x00000800;
    .gdt : {
        KEEP(*(.gdt))
    }

    /* Keep the multiboot header early in the kernel image (as before) */
    . = 0x00100000;  /* Kernel load base: 1 MiB */
    .multiboot : { *(.multiboot) }

    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss COMMON) }
}

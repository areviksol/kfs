OUTPUT_FORMAT("elf32-i386")
ENTRY(start)

SECTIONS
{
    . = 0x100000;  /* load at 1 MB */

    .multiboot : { *(.multiboot) }

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}

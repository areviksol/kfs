OUTPUT_FORMAT("elf32-i386") /* Выходной формат ELF для i386 (32-бит) */
ENTRY(start) /* Точка входа — метка start из boot.asm */

SECTIONS /* Разметка секций в адресном пространстве */
{
    . = 0x100000;  /* Базовый адрес загрузки ядра: 1 МБ */

    /* Multiboot header - must be in first 8KB of first LOAD segment */
    .multiboot : {
        *(.multiboot)
    }
    :text

    /* Code section - Read + Execute */
    .text : {
        *(.text)
    }
    :text

    /* Read-only data */
    .rodata : {
        *(.rodata)
    }
    :text

    /* Initialized data - Read + Write */
    .data : {
        *(.data)
    }
    :data

    /* Uninitialized data - Read + Write */
    .bss : {
        *(.bss)
    }
    :data

    /* GDT at fixed address - Read + Write (separate segment) */
    .gdt 0x00000800 : AT(0x00000800) {
        *(.gdt)
    }
    :gdt

    /DISCARD/ : {
        *(.comment)
        *(.note*)
    }
}

PHDRS {
    text PT_LOAD FLAGS(5); /* PF_R | PF_X (Read + Execute) = 0x5 */
    data PT_LOAD FLAGS(6); /* PF_R | PF_W (Read + Write) = 0x6 */
    gdt PT_LOAD FLAGS(6); /* PF_R | PF_W for GDT at fixed address */
}

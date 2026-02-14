#include <stdint.h> // Подключает фиксированные целочисленные типы стандартной библиотеки C (uint8_t, uint16_t и т. д.)
#include "types.h" // Типы ядра и запасной typedef для size_t на 32-бит
#include "lib.h" // Объявления базовых функций (strlen, strcmp, memset, memcpy)
#include "gdt.h" // GDT initialization
#include "printk.h" // printk and stack printing functions
#include "shell.h" // Debugging shell
#include "idt.h" // IDT initialization
#include "pic.h" // PIC initialization
#include "keyboard.h" // Keyboard support

static inline void outb(uint16_t port, uint8_t val) { // Запись одного байта в порт ввода/вывода (I/O)
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port)); // asm-инструкция outb: al -> [dx]
}

static inline uint8_t inb(uint16_t port) { // Чтение одного байта из порта ввода/вывода (I/O)
    uint8_t ret; // Регистр AL (через констрейнт "=a") вернёт считанный байт
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port)); // asm-инструкция inb: [dx] -> al
    return ret; // Возвращаем полученное значение
}

void kmain(uint32_t multiboot_magic, uint32_t multiboot_info_addr) { // Точка входа C-части ядра (вызывается из boot.asm)
    // Multiboot v1: EAX должен быть 0x2BADB002, EBX — адрес структуры multiboot_info
    if (multiboot_magic != 0x2BADB002) { // Проверка, что нас загрузил совместимый загрузчик
        while (1) {} // Неверный загрузчик — остановиться
    }
    (void)multiboot_info_addr; // Пока не используем структуру, подавить предупреждение
    
    /* Initialize the GDT */
    gdt_init();
    
    volatile uint16_t* vga = (uint16_t*)0xB8000; // Адрес текстового буфера VGA
    vga[0] = '4' | (0x0F << 8);  // Напечатать '4' атрибутом ярко-белый на чёрном
    vga[1] = '2' | (0x0F << 8);  // Напечатать '2' рядом

    /* Initialize and use printk */
    printk("\n========================================\n");
    printk("Kernel 42 - KFS-2\n");
    printk("========================================\n\n");
    
    printk("GDT initialized successfully!\n");
    printk("GDT Base Address: 0x00000800\n");
    printk("GDT Descriptors: %d\n", 7);
    printk("  - Null Descriptor\n");
    printk("  - Kernel Code Segment (0x08)\n");
    printk("  - Kernel Data Segment (0x10)\n");
    printk("  - Kernel Stack Segment (0x18)\n");
    printk("  - User Code Segment (0x23)\n");
    printk("  - User Data Segment (0x2B)\n");
    printk("  - User Stack Segment (0x33)\n\n");
    
    /* Display kernel stack information */
    print_stack();
    
    /* Initialize interrupts */
    printk("Initializing PIC...\n");
    pic_init();
    
    printk("Initializing IDT...\n");
    idt_init();
    
    /* Initialize keyboard */
    printk("Initializing keyboard...\n");
    keyboard_init();
    
    /* Enable interrupts */
    __asm__ volatile("sti");  /* Set Interrupt Flag */
    
    /* Initialize and start the debugging shell */
    shell_init();
    shell_main_loop();

    while(1); // Бесконечный цикл, чтобы ядро не завершилось
}


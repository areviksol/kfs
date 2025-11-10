#include <stdint.h> // Подключает фиксированные целочисленные типы стандартной библиотеки C (uint8_t, uint16_t и т. д.)
#include "types.h" // Типы ядра и запасной typedef для size_t на 32-бит
#include "lib.h" // Объявления базовых функций (strlen, strcmp, memset, memcpy)

static inline void outb(uint16_t port, uint8_t val) { // Запись одного байта в порт ввода/вывода (I/O)
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port)); // asm-инструкция outb: al -> [dx]
}

static inline uint8_t inb(uint16_t port) { // Чтение одного байта из порта ввода/вывода (I/O)
    uint8_t ret; // Регистр AL (через констрейнт "=a") вернёт считанный байт
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port)); // asm-инструкция inb: [dx] -> al
    return ret; // Возвращаем полученное значение
}

static void serial_init(void) { // Инициализация UART на порту COM1
    const uint16_t base = 0x3F8; // COM1 базовый адрес
    outb(base + 1, 0x00); // Отключить прерывания (IER = 0)
    outb(base + 3, 0x80); // Включить DLAB (доступ к делителям скорости)
    outb(base + 0, 0x03); // Делитель, младший байт: 3 (115200/3 = 38400 бод)
    outb(base + 1, 0x00); // Делитель, старший байт: 0
    outb(base + 3, 0x03); // 8 бит данных, без чётности, 1 стоп-бит (LCR)
    outb(base + 2, 0xC7); // Включить FIFO, очистить, триггер 14 байт (FCR)
    outb(base + 4, 0x0B); // Включить IRQ, установить RTS/DSR (MCR)
}

static void serial_write_char(char c) { // Записать один символ в UART
    const uint16_t base = 0x3F8; // COM1 базовый адрес
    // Ждать, пока передатчик готов (LSR бит 5 — THR Empty)
    while ((inb(base + 5) & 0x20) == 0) {} 
    outb(base + 0, (uint8_t)c); // Записать байт в регистр данных (THR)
}

static void serial_write(const char* s) { // Вывод строки в UART (CRLF для перевода строки)
    for (; *s; ++s) { // Идём по символам до нулевого байта
        if (*s == '\n') serial_write_char('\r'); // Перед \n добавить \r для CRLF
        serial_write_char(*s); // Отправить символ
    }
}

void kmain(uint32_t multiboot_magic, uint32_t multiboot_info_addr) { // Точка входа C-части ядра (вызывается из boot.asm)
    // Multiboot v1: EAX должен быть 0x2BADB002, EBX — адрес структуры multiboot_info
    if (multiboot_magic != 0x2BADB002) { // Проверка, что нас загрузил совместимый загрузчик
        while (1) {} // Неверный загрузчик — остановиться
    }
    (void)multiboot_info_addr; // Пока не используем структуру, подавить предупреждение
    volatile uint16_t* vga = (uint16_t*)0xB8000; // Адрес текстового буфера VGA
    vga[0] = '4' | (0x0F << 8);  // Напечатать '4' атрибутом ярко-белый на чёрном
    vga[1] = '2' | (0x0F << 8);  // Напечатать '2' рядом

    serial_init(); // Настроить COM1
    serial_write("42\n"); // Вывести "42" в сериал-консоль

    while(1); // Бесконечный цикл, чтобы ядро не завершилось
}

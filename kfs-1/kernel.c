#include <stdint.h>
#include "types.h"
#include "lib.h"

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void serial_init(void) {
    const uint16_t base = 0x3F8; // COM1
    outb(base + 1, 0x00); // disable interrupts
    outb(base + 3, 0x80); // enable DLAB
    outb(base + 0, 0x03); // divisor low (115200/3 = 38400 baud)
    outb(base + 1, 0x00); // divisor high
    outb(base + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(base + 2, 0xC7); // enable FIFO, clear, 14-byte threshold
    outb(base + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

static void serial_write_char(char c) {
    const uint16_t base = 0x3F8; // COM1
    // wait for transmitter empty
    while ((inb(base + 5) & 0x20) == 0) {} 
    outb(base + 0, (uint8_t)c);
}

static void serial_write(const char* s) {
    for (; *s; ++s) {
        if (*s == '\n') serial_write_char('\r');
        serial_write_char(*s);
    }
}

void kmain() {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    vga[0] = '4' | (0x0F << 8);  // bright white on black
    vga[1] = '2' | (0x0F << 8);

    serial_init();
    serial_write("42\n");

    while(1); // loop forever
}

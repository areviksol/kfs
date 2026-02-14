#include "printk.h"
#include "lib.h"

/* Serial port I/O functions (from kernel.c) */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void serial_init(void) {
    const uint16_t base = 0x3F8;
    outb(base + 1, 0x00);
    outb(base + 3, 0x80);
    outb(base + 0, 0x03);
    outb(base + 1, 0x00);
    outb(base + 3, 0x03);
    outb(base + 2, 0xC7);
    outb(base + 4, 0x0B);
}

static void serial_write_char(char c) {
    const uint16_t base = 0x3F8;
    while ((inb(base + 5) & 0x20) == 0) {}
    outb(base + 0, (uint8_t)c);
}

static void serial_write(const char *s) {
    for (; *s; ++s) {
        if (*s == '\n') serial_write_char('\r');
        serial_write_char(*s);
    }
}

/* Convert integer to hex string */
static void itohex(uint32_t value, char *buffer, int width) {
    const char *hex_chars = "0123456789ABCDEF";
    int i = width - 1;
    
    while (i >= 0) {
        buffer[i] = hex_chars[value & 0xF];
        value >>= 4;
        i--;
    }
    buffer[width] = '\0';
}

/* Convert integer to decimal string */
static void itodec(int32_t value, char *buffer) {
    if (value < 0) {
        *buffer++ = '-';
        value = -value;
    }
    
    char temp[12];
    int i = 0;
    
    if (value == 0) {
        temp[i++] = '0';
    } else {
        while (value > 0) {
            temp[i++] = '0' + (value % 10);
            value /= 10;
        }
    }
    
    while (i > 0) {
        *buffer++ = temp[--i];
    }
    *buffer = '\0';
}

/* Simple printk implementation with format support */
void printk(const char *fmt, ...) {
    static int initialized = 0;
    if (!initialized) {
        serial_init();
        initialized = 1;
    }
    
    va_list args;
    va_start(args, fmt);
    
    char buffer[256];
    int buf_pos = 0;
    
    while (*fmt && buf_pos < 255) {
        if (*fmt == '%' && *(fmt + 1)) {
            fmt++;
            if (*fmt == 'd') {
                int val = va_arg(args, int);
                char temp[12];
                itodec(val, temp);
                const char *p = temp;
                while (*p && buf_pos < 255) {
                    buffer[buf_pos++] = *p++;
                }
            } else if (*fmt == 'x') {
                uint32_t val = va_arg(args, uint32_t);
                char temp[9];
                itohex(val, temp, 8);
                const char *p = temp;
                while (*p && buf_pos < 255) {
                    buffer[buf_pos++] = *p++;
                }
            } else if (*fmt == 's') {
                const char *str = va_arg(args, const char *);
                while (*str && buf_pos < 255) {
                    buffer[buf_pos++] = *str++;
                }
            } else if (*fmt == 'c') {
                char c = va_arg(args, int);
                buffer[buf_pos++] = c;
            } else if (*fmt == '%') {
                buffer[buf_pos++] = '%';
            }
            fmt++;
        } else if (*fmt == '\\' && *(fmt + 1) == 'n') {
            buffer[buf_pos++] = '\n';
            fmt += 2;
        } else {
            buffer[buf_pos++] = *fmt++;
        }
    }
    
    buffer[buf_pos] = '\0';
    va_end(args);
    
    serial_write(buffer);
}

/* Print a single hex value with label */
void print_hex(const char *label, uint32_t value) {
    char hex_str[9];
    itohex(value, hex_str, 8);
    
    char buffer[128];
    buffer[0] = '\0';
    
    int pos = 0;
    const char *p = label;
    while (*p && pos < 120) {
        buffer[pos++] = *p++;
    }
    
    const char *hex = "0x";
    while (*hex && pos < 120) {
        buffer[pos++] = *hex++;
    }
    
    p = hex_str;
    while (*p && pos < 120) {
        buffer[pos++] = *p++;
    }
    
    buffer[pos++] = '\n';
    buffer[pos] = '\0';
    
    serial_write(buffer);
}

/* Get current stack pointer via inline assembly */
static uint32_t get_esp(void) {
    uint32_t esp;
    __asm__ volatile ("mov %%esp, %0" : "=r"(esp));
    return esp;
}

/* Get current base pointer via inline assembly */
static uint32_t get_ebp(void) {
    uint32_t ebp;
    __asm__ volatile ("mov %%ebp, %0" : "=r"(ebp));
    return ebp;
}

/* Print kernel stack information in human-friendly format */
void print_stack(void) {
    uint32_t esp = get_esp();
    uint32_t ebp = get_ebp();
    
    printk("\n========== KERNEL STACK INFO ==========\n");
    print_hex("Stack Pointer (ESP): ", esp);
    print_hex("Base Pointer (EBP): ", ebp);
    
    printk("\nStack Contents (first 16 entries):\n");
    uint32_t *stack_ptr = (uint32_t *)esp;
    int count = 0;
    
    while (count < 16 && (uint32_t)stack_ptr < 0xFFFFF000) {
        char addr_str[9];
        char val_str[9];
        char entry_line[64];
        
        itohex((uint32_t)stack_ptr, addr_str, 8);
        itohex(*stack_ptr, val_str, 8);
        
        int pos = 0;
        entry_line[pos++] = '[';
        const char *p = addr_str;
        while (*p && pos < 60) entry_line[pos++] = *p++;
        entry_line[pos++] = ']';
        entry_line[pos++] = ' ';
        entry_line[pos++] = '=';
        entry_line[pos++] = ' ';
        entry_line[pos++] = '0';
        entry_line[pos++] = 'x';
        p = val_str;
        while (*p && pos < 60) entry_line[pos++] = *p++;
        entry_line[pos++] = '\n';
        entry_line[pos] = '\0';
        
        serial_write(entry_line);
        
        stack_ptr++;
        count++;
    }
    
    printk("========================================\n\n");
}

/* kernel.c -- uses printk() (printf-style) and print_stack() */
#include <stdint.h>
#include <stdarg.h>
#include "types.h"
#include "lib.h"
#include "printf.h"

/* gdt init from gdt.asm */
extern void gdt_init(void);

/* boot.asm defines these labels (stack_space/resb and stack_end) */
extern uint8_t stack_space;
extern uint8_t stack_end;

/* I/O helpers (same as your previous implementations) */
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

/* Serial writer: writes without automatic newline, but preserves CRLF behavior for \\n */
static void serial_write_char(char c) {
    const uint16_t base = 0x3F8;
    while ((inb(base + 5) & 0x20) == 0) { }
    outb(base + 0, (uint8_t)c);
}

static void serial_write(const char* s) {
    for (; *s; ++s) {
        if (*s == '\n') serial_write_char('\r');
        serial_write_char(*s);
    }
}

/* VGA simple writer */
static volatile uint16_t* const VGABUF = (uint16_t*)0xB8000;
static int vga_row = 0, vga_col = 0;
static const uint8_t VGA_ATTR = 0x07;

static void vga_putc(char c) {
    if (c == '\n') { vga_col = 0; vga_row++; return; }
    VGABUF[vga_row*80 + vga_col] = ((uint16_t)VGA_ATTR << 8) | (uint8_t)c;
    vga_col++;
    if (vga_col >= 80) { vga_col = 0; vga_row++; }
}

/* printk: format into buffer, write to VGA and serial */
int printk(const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, ap);  // make sure vsnprintf is in printf.c
    va_end(ap);

    for (int i=0; i<len; i++) vga_putc(buf[i]);
    serial_write(buf);
    return len;
}


/* Set SS:ESP to kernel stack (stack_end label) using kernel stack selector 0x18 */
void set_kernel_stack_and_enter(void) {
    uint32_t stack_top = (uint32_t)&stack_end;
    asm volatile (
        "movw $0x18, %%ax\n"
        "movw %%ax, %%ss\n"
        "movl %0, %%esp\n"
        : : "r"(stack_top) : "ax"
    );
}

/* Walk saved EBP frames and print a human-friendly trace */
void print_stack(void) {
    uint32_t *ebp;
    asm volatile ("movl %%ebp, %0" : "=r"(ebp));

    printk("--- kernel stack trace ---");
    int frame = 0;
    while (ebp && frame < 5) {
        uint32_t saved_ebp = ebp[0];
        uint32_t ret = ebp[1];
        printk("frame %02d: ebp=%p ret=%p", frame, ebp, (void*)ret);
        frame++;
        if (saved_ebp == 0 || saved_ebp == (uint32_t)ebp) break;
        ebp = (uint32_t*)saved_ebp;
    }
    printk("--- end stack ---");
}

/* kernel entrypoint */
void kmain(uint32_t multiboot_magic, uint32_t multiboot_info_addr) {
    (void)multiboot_info_addr;

    if (multiboot_magic != 0x2BADB002) {
        while (1) { asm volatile("hlt"); }
    }

    serial_init();

    printk("Calling gdt_init()...\n");
    gdt_init();
    printk("gdt_init() returned.\n");

    // Set kernel stack
    set_kernel_stack_and_enter();
    printk("Kernel stack set (ss=0x18) at ESP=%p\n", &stack_end);

    // Comment out print_stack() for now
    // print_stack();

    while (1) { asm volatile("hlt"); }
}

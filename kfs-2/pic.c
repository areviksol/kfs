#include "pic.h"
#include "lib.h"

/* I/O Port operations */
static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a" (value), "dN" (port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile("inb %1, %0" : "=a" (result) : "dN" (port));
    return result;
}

/**
 * Initialize the 8259A Programmable Interrupt Controller
 * Maps IRQs 0-7 (master) and 8-15 (slave) to ISRs 32-47
 */
void pic_init(void) {
    /* Start initialization sequence with ICW1 */
    outb(PIC_MASTER_CMD, ICW1);
    outb(PIC_SLAVE_CMD, ICW1);

    /* ICW2: Vector offset for master (IRQ0-7 map to ISR32-39) */
    outb(PIC_MASTER_DATA, 32);
    
    /* ICW2: Vector offset for slave (IRQ8-15 map to ISR40-47) */
    outb(PIC_SLAVE_DATA, 40);

    /* ICW3: Cascade mode - IRQ2 on master connects to slave */
    outb(PIC_MASTER_DATA, 0x04);
    outb(PIC_SLAVE_DATA, 0x02);

    /* ICW4: 8086 mode, auto EOI */
    outb(PIC_MASTER_DATA, ICW4);
    outb(PIC_SLAVE_DATA, ICW4);

    /* Enable IRQ0 (timer) and IRQ1 (keyboard), disable all others */
    outb(PIC_MASTER_DATA, PIC_IRQ01);  /* Enable IRQ0 and IRQ1 on master */
    outb(PIC_SLAVE_DATA, 0xFF);        /* Disable all IRQs on slave */
}

/**
 * Send End of Interrupt to the PIC
 */
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        /* Slave PIC IRQ */
        outb(PIC_SLAVE_CMD, PIC_EOI);
    }
    /* Always send to master PIC */
    outb(PIC_MASTER_CMD, PIC_EOI);
}

/**
 * Enable specific IRQ
 */
void pic_enable_irq(uint8_t irq) {
    uint8_t mask;
    uint16_t port;

    if (irq < 8) {
        port = PIC_MASTER_DATA;
        mask = inb(port);
        mask &= ~(1 << irq);
        outb(port, mask);
    } else {
        port = PIC_SLAVE_DATA;
        mask = inb(port);
        mask &= ~(1 << (irq - 8));
        outb(port, mask);
    }
}

/**
 * Disable specific IRQ
 */
void pic_disable_irq(uint8_t irq) {
    uint8_t mask;
    uint16_t port;

    if (irq < 8) {
        port = PIC_MASTER_DATA;
        mask = inb(port);
        mask |= (1 << irq);
        outb(port, mask);
    } else {
        port = PIC_SLAVE_DATA;
        mask = inb(port);
        mask |= (1 << (irq - 8));
        outb(port, mask);
    }
}

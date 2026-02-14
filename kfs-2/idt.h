#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* IDT Gate Descriptor Structure (8 bytes) */
struct idt_gate {
    uint16_t offset_low;      /* Low 16 bits of handler address */
    uint16_t selector;        /* Code segment selector (0x08 for kernel code) */
    uint8_t  reserved;        /* Reserved, set to 0 */
    uint8_t  attributes;      /* Gate type and attributes */
    uint16_t offset_high;     /* High 16 bits of handler address */
} __attribute__((packed));

/* IDT Register (loaded with LIDT) */
struct idtr {
    uint16_t size;            /* Size of IDT in bytes - 1 */
    uint32_t address;         /* 32-bit address of IDT */
} __attribute__((packed));

/* Gate Type Attributes */
#define IDT_GATE_INTERRUPT  0x0E  /* 32-bit interrupt gate */
#define IDT_GATE_TRAP       0x0F  /* 32-bit trap gate */
#define IDT_GATE_TASK       0x05  /* Task gate */

/* Privilege Level */
#define IDT_DPL_KERNEL      0x00  /* Ring 0 */
#define IDT_DPL_USER        0x60  /* Ring 3 */

/* Presence Bit */
#define IDT_PRESENT         0x80

/* IDT Sizes */
#define IDT_ENTRIES         256
#define IDT_SIZE            (IDT_ENTRIES * sizeof(struct idt_gate))

/* Function Declarations */
void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t handler, uint8_t type, uint8_t dpl);
void idt_load(void);

/* Interrupt Handler Declarations */
extern void isr0_handler(void);   /* Division by zero */
extern void isr8_handler(void);   /* Double fault */
extern void irq0_handler(void);   /* PIT (timer) */
extern void irq1_handler(void);   /* PS/2 Keyboard */

#endif /* IDT_H */

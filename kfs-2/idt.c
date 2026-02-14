#include "idt.h"
#include "lib.h"

/* IDT Table - 256 entries for all interrupts */
static struct idt_gate idt[IDT_ENTRIES] __attribute__((section(".data")));

/* IDT Register */
static struct idtr idtr_value;

/**
 * Set an IDT gate descriptor
 */
void idt_set_gate(uint8_t num, uint32_t handler, uint8_t type, uint8_t dpl) {
    /* num is uint8_t (0-255), IDT_ENTRIES is 256, so no bounds check needed */
    uint32_t addr = handler;

    idt[num].offset_low = (uint16_t)(addr & 0xFFFF);
    idt[num].offset_high = (uint16_t)((addr >> 16) & 0xFFFF);
    idt[num].selector = 0x08;  /* Kernel code segment */
    idt[num].reserved = 0;
    idt[num].attributes = type | dpl | IDT_PRESENT;
}

/**
 * Initialize the IDT
 * Sets up exception handlers and interrupt handlers
 */
void idt_init(void) {
    /* Clear the IDT */
    memset(idt, 0, sizeof(idt));

    /* Set up the IDTR */
    idtr_value.size = (IDT_SIZE - 1);
    idtr_value.address = (uint32_t)&idt[0];

    /* Install exception handlers */
    idt_set_gate(0, (uint32_t)isr0_handler, IDT_GATE_INTERRUPT, IDT_DPL_KERNEL);
    idt_set_gate(8, (uint32_t)isr8_handler, IDT_GATE_INTERRUPT, IDT_DPL_KERNEL);

    /* Install interrupt handlers */
    idt_set_gate(32, (uint32_t)irq0_handler, IDT_GATE_INTERRUPT, IDT_DPL_KERNEL);  /* Timer (IRQ0) */
    idt_set_gate(33, (uint32_t)irq1_handler, IDT_GATE_INTERRUPT, IDT_DPL_KERNEL);  /* Keyboard (IRQ1) */

    /* Load IDT into processor */
    idt_load();
}

/**
 * Load IDTR into processor (implemented in idt_load.asm)
 */
extern void idt_load_register(struct idtr *idtr);

void idt_load(void) {
    idt_load_register(&idtr_value);
}

#ifndef PIC_H
#define PIC_H

#include <stdint.h>

/* PIC Port Addresses */
#define PIC_MASTER_CMD   0x20
#define PIC_MASTER_DATA  0x21
#define PIC_SLAVE_CMD    0xA0
#define PIC_SLAVE_DATA   0xA1

/* PIC Commands */
#define PIC_EOI          0x20    /* End of Interrupt */
#define ICW1             0x11    /* Initialization Command Word 1 */
#define ICW4             0x01    /* Initialization Command Word 4 */

/* PIC Masks - which IRQs are enabled */
#define PIC_IRQ0         0xFE    /* Enable IRQ0 (timer), disable others */
#define PIC_IRQ01        0xFC    /* Enable IRQ0 and IRQ1 (timer + keyboard) */

/* Function Declarations */
void pic_init(void);
void pic_send_eoi(uint8_t irq);
void pic_enable_irq(uint8_t irq);
void pic_disable_irq(uint8_t irq);

#endif /* PIC_H */

#ifndef GDT_H
#define GDT_H

#include <stdint.h>

/* GDT Descriptor Structure (8 bytes per descriptor) */
struct gdt_descriptor {
    uint16_t limit_low;      /* Bits 0-15 of segment limit */
    uint16_t base_low;       /* Bits 0-15 of segment base */
    uint8_t  base_mid;       /* Bits 16-23 of segment base */
    uint8_t  access_byte;    /* Type and access flags */
    uint8_t  granularity;    /* Granularity and upper limit bits */
    uint8_t  base_high;      /* Bits 24-31 of segment base */
} __attribute__((packed));

/* GDTR (GDT Register) structure for LGDT instruction */
struct gdtr {
    uint16_t limit;          /* Size of GDT - 1 */
    uint32_t base;           /* Linear address of GDT */
} __attribute__((packed));

/* GDT Descriptor Access Byte Flags */
#define GDT_ACCESS_PRESENT     0x80  /* Segment present in memory */
#define GDT_ACCESS_DPL_0       0x00  /* Privilege level 0 (kernel) */
#define GDT_ACCESS_DPL_3       0x60  /* Privilege level 3 (user) */
#define GDT_ACCESS_S_CODE_DATA 0x10  /* Code or data segment (not system) */
#define GDT_ACCESS_EXECUTABLE  0x08  /* Code segment (executable) */
#define GDT_ACCESS_DC_DOWN     0x04  /* Direction (0=up for stack) */
#define GDT_ACCESS_RW          0x02  /* Readable/Writable */
#define GDT_ACCESS_ACCESSED    0x01  /* Accessed bit */

/* GDT Descriptor Granularity Byte Flags */
#define GDT_GRAN_PAGE_SIZE     0x80  /* Granularity: 4KB pages */
#define GDT_GRAN_32BIT         0x40  /* 32-bit protected mode */
#define GDT_GRAN_16BIT         0x00  /* 16-bit real mode */

/* GDT Segment Indices (for selector values) */
#define GDT_KERNEL_CODE_INDEX  1
#define GDT_KERNEL_DATA_INDEX  2
#define GDT_KERNEL_STACK_INDEX 3
#define GDT_USER_CODE_INDEX    4
#define GDT_USER_DATA_INDEX    5
#define GDT_USER_STACK_INDEX   6
#define GDT_TOTAL_DESCRIPTORS  7

/* Selector values (index * 8) */
#define GDT_KERNEL_CODE_SELECTOR  (GDT_KERNEL_CODE_INDEX << 3)
#define GDT_KERNEL_DATA_SELECTOR  (GDT_KERNEL_DATA_INDEX << 3)
#define GDT_KERNEL_STACK_SELECTOR (GDT_KERNEL_STACK_INDEX << 3)
#define GDT_USER_CODE_SELECTOR    ((GDT_USER_CODE_INDEX << 3) | 3)
#define GDT_USER_DATA_SELECTOR    ((GDT_USER_DATA_INDEX << 3) | 3)
#define GDT_USER_STACK_SELECTOR   ((GDT_USER_STACK_INDEX << 3) | 3)

/* GDT initialization function */
void gdt_init(void);

/* GDT load function (defined in gdt_load.asm) */
extern void gdt_load(struct gdtr *gdtr);

#endif /* GDT_H */

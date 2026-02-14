#include "gdt.h"
#include "lib.h"

/* Global GDT array at fixed address 0x00000800 */
static struct gdt_descriptor gdt[GDT_TOTAL_DESCRIPTORS] __attribute__((section(".gdt")));

/* Helper function to fill a GDT descriptor */
static void gdt_set_descriptor(
    uint32_t index,
    uint32_t base,
    uint32_t limit,
    uint8_t access,
    uint8_t granularity
) {
    if (index >= GDT_TOTAL_DESCRIPTORS) return;

    gdt[index].base_low    = (base & 0xFFFF);
    gdt[index].base_mid    = (base >> 16) & 0xFF;
    gdt[index].base_high   = (base >> 24) & 0xFF;

    gdt[index].limit_low   = (limit & 0xFFFF);
    gdt[index].granularity = ((limit >> 16) & 0x0F) | granularity;

    gdt[index].access_byte = access;
}

void gdt_init(void) {
    /* Null descriptor (required, selector 0x00) */
    gdt_set_descriptor(0, 0x00000000, 0x00000000,
                       0x00, 0x00);

    /* Kernel Code Segment */
    /* Base: 0x00000000, Limit: 0xFFFFF (4GB with page granularity) */
    gdt_set_descriptor(GDT_KERNEL_CODE_INDEX,
                       0x00000000, 0xFFFFF,
                       GDT_ACCESS_PRESENT | GDT_ACCESS_DPL_0 | GDT_ACCESS_S_CODE_DATA | 
                       GDT_ACCESS_EXECUTABLE | GDT_ACCESS_RW,
                       GDT_GRAN_PAGE_SIZE | GDT_GRAN_32BIT);

    /* Kernel Data Segment */
    /* Base: 0x00000000, Limit: 0xFFFFF (4GB with page granularity) */
    gdt_set_descriptor(GDT_KERNEL_DATA_INDEX,
                       0x00000000, 0xFFFFF,
                       GDT_ACCESS_PRESENT | GDT_ACCESS_DPL_0 | GDT_ACCESS_S_CODE_DATA | 
                       GDT_ACCESS_RW,
                       GDT_GRAN_PAGE_SIZE | GDT_GRAN_32BIT);

    /* Kernel Stack Segment */
    /* Base: 0x00000000, Limit: 0xFFFFF (4GB with page granularity) */
    gdt_set_descriptor(GDT_KERNEL_STACK_INDEX,
                       0x00000000, 0xFFFFF,
                       GDT_ACCESS_PRESENT | GDT_ACCESS_DPL_0 | GDT_ACCESS_S_CODE_DATA | 
                       GDT_ACCESS_DC_DOWN | GDT_ACCESS_RW,
                       GDT_GRAN_PAGE_SIZE | GDT_GRAN_32BIT);

    /* User Code Segment */
    /* Base: 0x00000000, Limit: 0xFFFFF (4GB with page granularity) */
    gdt_set_descriptor(GDT_USER_CODE_INDEX,
                       0x00000000, 0xFFFFF,
                       GDT_ACCESS_PRESENT | GDT_ACCESS_DPL_3 | GDT_ACCESS_S_CODE_DATA | 
                       GDT_ACCESS_EXECUTABLE | GDT_ACCESS_RW,
                       GDT_GRAN_PAGE_SIZE | GDT_GRAN_32BIT);

    /* User Data Segment */
    /* Base: 0x00000000, Limit: 0xFFFFF (4GB with page granularity) */
    gdt_set_descriptor(GDT_USER_DATA_INDEX,
                       0x00000000, 0xFFFFF,
                       GDT_ACCESS_PRESENT | GDT_ACCESS_DPL_3 | GDT_ACCESS_S_CODE_DATA | 
                       GDT_ACCESS_RW,
                       GDT_GRAN_PAGE_SIZE | GDT_GRAN_32BIT);

    /* User Stack Segment */
    /* Base: 0x00000000, Limit: 0xFFFFF (4GB with page granularity) */
    gdt_set_descriptor(GDT_USER_STACK_INDEX,
                       0x00000000, 0xFFFFF,
                       GDT_ACCESS_PRESENT | GDT_ACCESS_DPL_3 | GDT_ACCESS_S_CODE_DATA | 
                       GDT_ACCESS_DC_DOWN | GDT_ACCESS_RW,
                       GDT_GRAN_PAGE_SIZE | GDT_GRAN_32BIT);

    /* Load the GDT register */
    struct gdtr gdtr_value;
    gdtr_value.base = (uint32_t)&gdt[0];
    gdtr_value.limit = (sizeof(struct gdt_descriptor) * GDT_TOTAL_DESCRIPTORS) - 1;

    gdt_load(&gdtr_value);
}

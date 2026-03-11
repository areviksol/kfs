# KFS-2

`kfs-2` is the second kernel stage: segmentation, interrupts, and interactive debugging.

## Implemented

- **GDT** (`gdt.c`, `gdt.h`) with kernel/user segments
- **IDT** (`idt.c`, `idt.h`, `idt_load.asm`)
- **PIC** (`pic.c`, `pic.h`) interrupt controller initialization
- **Keyboard** (`keyboard.c`, `keyboard.h`) input handling
- **Shell** (`shell.c`, `shell.h`) simple command loop
- **printk/printf** for debug output

## Build and Run

```bash
make clean
make iso
make run
```

## Docker Option

```bash
make docker-image
make docker-iso
make docker-run
```

## Initialization Order

1. Validate Multiboot boot
2. `gdt_init()`
3. `pic_init()`
4. `idt_init()`
5. `keyboard_init()`
6. `sti` (enable interrupts)
7. start shell (`shell_main_loop()`)

## Core Files

- `kernel.c` - central startup sequence
- `boot.asm` - early code before entering `kmain`
- `grub.cfg` - GRUB configuration
- `linker.ld` - kernel memory layout
- `Makefile` - build, ISO, and QEMU run targets

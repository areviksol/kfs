# KFS-1

`kfs-1` is the first kernel stage: Multiboot startup, basic VGA output, and COM1 serial output.

## Implemented

- `multiboot_magic` validation in `kmain`
- printing `42` to the VGA text buffer (`0xB8000`)
- COM1 (`0x3F8`) initialization and serial output
- minimal utility functions in `lib.c`

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

## Files

- `boot.asm` - early initialization and jump to C code
- `kernel.c` - main stage logic
- `grub.cfg` - bootloader configuration
- `linker.ld` - linker script
- `Makefile` - build commands

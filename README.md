# KFS

`KFS` is an educational project focused on building a 32-bit i386 kernel, booted by GRUB and run in QEMU.

## Repository Structure

- `kfs-1` - basic stage: kernel boot, VGA/COM1 output, minimal libc.
- `kfs-2` - extended stage: GDT, IDT, PIC, keyboard, and a simple shell.

## Requirements

- `nasm`
- `qemu-system-i386`
- `i686-elf-*` cross-toolchain (`gcc`, `ld`, `grub-mkrescue`)
- optional: `docker` and `docker buildx`

## Quick Start

### Stage 1

```bash
cd kfs-1
make clean
make iso
make run
```

### Stage 2

```bash
cd kfs-2
make clean
make iso
make run
```

## Docker Run

Both subprojects include `docker-image`, `docker-iso`, and `docker-run` targets, so you can build without installing the toolchain locally.

## Notes

- Kernel entry point: `kmain` in `kernel.c`.
- Bootloader configuration: `grub.cfg`.
- Section layout: `linker.ld`.

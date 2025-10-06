# KFS - Kernel From Scratch

A minimal operating system kernel written from scratch in C and assembly.

## Description

KFS is a simple x86 kernel that demonstrates the basic concepts of operating system development. It includes:

- Multiboot-compliant bootloader
- VGA text mode driver for console output
- Basic kernel initialization and setup

## Prerequisites

To build and run KFS, you need:

- `nasm` - The Netwide Assembler
- `gcc` - GNU Compiler Collection (with 32-bit support)
- `ld` - GNU linker
- `qemu-system-i386` - (Optional) For testing the kernel

### Installing Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get install nasm gcc make qemu-system-x86
sudo apt-get install gcc-multilib  # For 32-bit compilation support
```

On Fedora/RHEL:
```bash
sudo dnf install nasm gcc make qemu-system-x86
sudo dnf install glibc-devel.i686  # For 32-bit compilation support
```

## Building

To build the kernel:

```bash
make
```

This will create `kfs.bin`, the kernel binary.

## Running

### Using QEMU

The easiest way to test the kernel is using QEMU:

```bash
make run
```

This will boot the kernel in QEMU and display the welcome message.

### Creating an ISO

To create a bootable ISO image (requires `grub-mkrescue`):

```bash
make iso
```

This creates `kfs.iso` which can be booted in a virtual machine or burned to a CD/USB.

### Using VirtualBox or VMware

1. Create the ISO: `make iso`
2. Create a new virtual machine with the ISO as the boot medium
3. Start the virtual machine

## Cleaning

To remove build artifacts:

```bash
make clean
```

## Project Structure

```
kfs/
├── boot.asm      # Assembly bootloader with Multiboot header
├── kernel.c      # Main kernel code with VGA text driver
├── linker.ld     # Linker script for memory layout
├── Makefile      # Build system
└── README.md     # This file
```

## How It Works

1. **Bootloader (boot.asm)**: The bootloader is compliant with the Multiboot specification, which allows it to be loaded by GRUB or QEMU. It sets up a stack and calls the kernel's main function.

2. **Kernel (kernel.c)**: The kernel initializes the VGA text mode buffer and writes a welcome message to the screen.

3. **Linker Script (linker.ld)**: The linker script defines how the kernel is laid out in memory, placing it at the 1 MiB mark as required by Multiboot.

## License

This project is provided as-is for educational purposes.
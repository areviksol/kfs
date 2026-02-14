# KFS-2: Kernel Stack and GDT Implementation

## Overview

This project implements a complete Global Descriptor Table (GDT) and kernel stack for a 32-bit x86 kernel. The GDT defines memory segments for kernel and user space, and is properly integrated with the bootloader via GRUB.

## Architecture

### Global Descriptor Table (GDT)

The GDT is placed at the fixed address **0x00000800** as required and contains the following descriptors:

1. **Null Descriptor (0x00)** - Required by CPU specification, always zero
2. **Kernel Code Segment (0x08)** - Ring 0, executable, base=0x00000000, limit=4GB
3. **Kernel Data Segment (0x10)** - Ring 0, writable, base=0x00000000, limit=4GB
4. **Kernel Stack Segment (0x18)** - Ring 0, writable, grows downward, base=0x00000000, limit=4GB
5. **User Code Segment (0x23)** - Ring 3, executable, base=0x00000000, limit=4GB
6. **User Data Segment (0x2B)** - Ring 3, writable, base=0x00000000, limit=4GB
7. **User Stack Segment (0x33)** - Ring 3, writable, grows downward, base=0x00000000, limit=4GB

Each selector value is calculated as `(descriptor_index << 3) | privilege_level`.

### Stack Implementation

The kernel stack is allocated in the `.bss` section of boot.asm:

```asm
section .bss
align 16
stack_space:    resb 8192      ; 8 KB kernel stack
stack_end:                       ; Stack pointer initialized here (top of stack)
```

Key characteristics:
- **Size**: 8 KB (8192 bytes)
- **Alignment**: 16 bytes (ABI compliant)
- **Growth**: Downward (from high address to low address, standard x86 behavior)
- **Initialization**: ESP is set to `stack_end` in boot.asm before calling kmain()

### Descriptor Structure

Each GDT descriptor is 8 bytes:

```
Byte 0-1:  Base Address (bits 0-15)
Byte 2:    Base Address (bits 16-23)
Byte 3:    Access Byte
           - P: Present (1 bit)
           - DPL: Descriptor Privilege Level (2 bits)
           - S: Descriptor type (1 bit)
           - Type: Segment type (4 bits)
Byte 4:    Granularity & Limit High
           - G: Granularity (1 bit) - 0=byte, 1=4KB page
           - DB: Default operand size (1 bit) - 0=16bit, 1=32bit
           - L: Long mode (1 bit) - 0=not 64-bit
           - Limit (bits 16-19)
Byte 5:    Base Address (bits 24-31)
```

## File Structure

### New Files

- **gdt.h** - GDT data structures and constants
- **gdt.c** - GDT initialization routine (C)
- **gdt.asm** - GDT loading routine (Assembly)
- **printk.h** - Printf-like formatted printing functions
- **printk.c** - Printk implementation with stack display

### Modified Files

- **kernel.c** - Updated to initialize GDT and print kernel information
- **linker.ld** - Updated to place GDT at 0x00000800
- **boot.asm** - Already correctly initializes stack pointer

## Building and Running

### Using Make

```bash
# Build the kernel (cross-compiler required)
make all

# Create bootable ISO
make iso

# Run in QEMU
make run
```

### Using Docker

```bash
# Build Docker image with toolchain
make docker-image

# Build ISO inside Docker
make docker-iso

# Run kernel in Docker with QEMU
make docker-run
```

## GDT Initialization Process

1. **Assembly Phase (boot.asm)**:
   - CPU is in 32-bit protected mode (set by GRUB)
   - Stack pointer is initialized to 0x101800 (stack_end)
   - No GDT is loaded yet (using BIOS/GRUB default)
   - Multiboot magic (0x2BADB002) and info pointer are passed to kmain()

2. **C Phase (kernel.c - kmain)**:
   - Verify Multiboot magic
   - Call `gdt_init()` to initialize all descriptors
   - Load GDTR and update segment selectors

3. **GDT Loading (gdt.asm - gdt_load)**:
   - Execute `LGDT` instruction with GDTR
   - Execute far jump to reload CS (code segment)
   - Reload DS, SS, ES, FS, GS with kernel data selector

## Printing Functions

### printk()

Printf-like formatted printing to serial console:

```c
printk("Format string with %%d, %%x, %%s, %%c\n", args...);
```

Supported format specifiers:
- `%d` - Decimal integer
- `%x` - Hexadecimal (8 digits)
- `%s` - String
- `%c` - Character
- `%%` - Literal percent sign

### print_stack()

Displays kernel stack information:

```
========== KERNEL STACK INFO ==========
Stack Pointer (ESP): 0x00101800
Base Pointer (EBP): 0x00101800

Stack Contents (first 16 entries):
[00101800] = 0x2BADB002
[00101804] = 0x00010000
...
========================================
```

## Compilation Flags

The kernel is compiled with these flags:
- `-m32` - 32-bit architecture
- `-ffreestanding` - No standard library assumptions
- `-fno-stack-protector` - Disable stack canary
- `-nostdlib` - Don't link with standard library
- `-Wall -Wextra` - Enable all warnings

Linker constraints:
- Can use `i686-elf-ld` but NOT the host system's linker
- Cannot use host system libraries (`-nodefaultlibs`, `-nostdlib`)
- Must use custom linker script (linker.ld)

## Memory Layout

```
0x00000000 -----> +----------------------------------+
                  |       Real Mode IVT (unused)    |
0x00000800 -----> +---> GDT (0.5 KB)              |
0x00001000 -----> +----------------------------------+
                  |       Kernel Stack (8 KB)      |
0x00003000 -----> +---> Stack grows downward      |
                  |
0x00100000 -----> +------ Kernel Load Address (1 MB)
                  | .multiboot section (4 bytes)
                  | .text section (code)
                  | .data section (data)
                  | .bss section (8 KB stack)
                  |
MEMORY_END -----> +----------------------------------+
```

## Segment Selectors

Segment selectors are 16-bit values:

```
Bits 0-1:   Privilege Level (Ring 0-3)
Bits 2:     Table Indicator (0=GDT, 1=LDT)
Bits 3-15:  Descriptor Index
```

Examples:
- `0x08` (0b0000_0000_0000_1000) = GDT index 1, ring 0 (kernel code)
- `0x10` (0b0000_0000_0001_0000) = GDT index 2, ring 0 (kernel data)
- `0x23` (0b0000_0000_0010_0011) = GDT index 4, ring 3 (user code)

## References

- OSDev Wiki: https://wiki.osdev.org/
- GDT Documentation: https://wiki.osdev.org/Global_Descriptor_Table
- x86 Memory Protection: https://wiki.osdev.org/Memory_Protection

## Status

✅ GDT implemented and placed at 0x00000800
✅ Kernel stack allocated (8 KB)
✅ Stack properly initialized in protected mode
✅ All 6 required segments (kernel code/data/stack + user code/data/stack)
✅ GDT loading mechanism implemented
✅ Printk with format specifiers
✅ Stack display function
✅ Total code size < 10 MB ✓

## Next Steps

1. Interrupt Descriptor Table (IDT) implementation
2. Exception handling and ISRs
3. Memory paging setup
4. User mode transitions
5. System call interface

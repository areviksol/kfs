# KFS-2 Quick Start Guide

## What Was Implemented

You now have a complete kernel with:
- ✅ **Global Descriptor Table (GDT)** - Memory segmentation for kernel and user space
- ✅ **Kernel Stack** - 8 KB stack for kernel execution  
- ✅ **Printf Function** - Formatted output to serial console
- ✅ **Stack Display** - Human-friendly kernel stack viewer

## Files Overview

### Core Implementation
| File | Purpose |
|------|---------|
| `gdt.h` | GDT structures and constants |
| `gdt.c` | GDT initialization (7 segments) |
| `gdt.asm` | GDT loading and segment reload |
| `printk.h` | Printf interface |
| `printk.c` | Printf implementation with serial I/O |
| `kernel.c` | **MODIFIED** - Now initializes GDT and prints info |
| `linker.ld` | **MODIFIED** - GDT placed at 0x00000800 |

### Documentation
| File | Contents |
|------|----------|
| `GDT_IMPLEMENTATION.md` | Detailed technical documentation |
| `GDT_DESCRIPTOR_FORMAT.md` | GDT descriptor format reference |
| `IMPLEMENTATION_SUMMARY.md` | Overview of what was done |
| `VERIFICATION_CHECKLIST.md` | Completeness verification |

## Building the Kernel

### Option 1: Direct Compilation (Requires Cross-Compiler)
```bash
cd /Users/arevikmkrtchyan/Desktop/42/kfs-2
make clean
make all
make iso
make run
```

### Option 2: Using Docker (No Prerequisites)
```bash
cd /Users/arevikmkrtchyan/Desktop/42/kfs-2
make docker-image
make docker-iso
make docker-run
```

## Expected Output

When you run the kernel, you'll see on the serial console:

```
========================================
Kernel 42 - KFS-2
========================================

GDT initialized successfully!
GDT Base Address: 0x00000800
GDT Descriptors: 7
  - Null Descriptor
  - Kernel Code Segment (0x08)
  - Kernel Data Segment (0x10)
  - Kernel Stack Segment (0x18)
  - User Code Segment (0x23)
  - User Data Segment (0x2B)
  - User Stack Segment (0x33)

========== KERNEL STACK INFO ==========
Stack Pointer (ESP): 0x00101800
Base Pointer (EBP): 0x00101800

Stack Contents (first 16 entries):
[00101800] = 0x2BADB002
[00101804] = 0x00010000
...
========================================
```

## Key Features

### 1. Global Descriptor Table
- **Location**: 0x00000800 (fixed address)
- **Size**: 56 bytes (7 descriptors × 8 bytes each)
- **Segments**: 
  - Kernel (code, data, stack) - Ring 0
  - User (code, data, stack) - Ring 3
  - Plus null descriptor

### 2. Kernel Stack
- **Size**: 8 KB (8192 bytes)
- **Address**: ~0x101800 (top)
- **Growth**: Downward (standard x86)
- **Alignment**: 16 bytes (ABI compliant)

### 3. Printf Function
```c
// Format specifiers: %d, %x, %s, %c, %%
printk("Decimal: %d, Hex: 0x%x, String: %s\n", 42, 255, "hello");
```

### 4. Stack Printer
```c
// Displays ESP, EBP, and first 16 stack entries
print_stack();
```

## Memory Layout

```
0x00000800 ── GDT (56 bytes)
0x00100000 ── Kernel Load Address
             ├─ .multiboot header
             ├─ .text (code)
             ├─ .data (data)
             └─ .bss (8 KB stack)
                 └─ ESP points here after init
```

## GDT Segment Selectors

```
Kernel Code:   0x08
Kernel Data:   0x10
Kernel Stack:  0x18
User Code:     0x23
User Data:     0x2B
User Stack:    0x33
```

Each selector = (descriptor_index << 3) | privilege_level

## How It Works

### Boot Sequence
1. **GRUB** loads kernel, sets CPU to 32-bit protected mode
2. **boot.asm** initializes ESP to 0x101800 (top of stack)
3. **kernel.c:kmain()** calls `gdt_init()` to set up descriptors
4. **gdt.c:gdt_load()** executes LGDT and reloads segments
5. **printk()** outputs initialization messages
6. **print_stack()** displays kernel stack information

### GDT Initialization
```c
gdt_set_descriptor(
    1,                              // Index 1
    0x00000000,                     // Base address
    0xFFFFF,                        // Limit (4GB with page granularity)
    GDT_ACCESS_PRESENT | ... ,     // Access byte
    GDT_GRAN_PAGE_SIZE | ...       // Granularity
);
```

### GDT Loading
```asm
; Load GDTR with GDT location
lgdt [eax]

; Far jump to reload CS
jmp 0x08:.flush

; Reload all data segment selectors
mov ds, eax  ; EAX = 0x10 (kernel data selector)
mov ss, eax
mov es, eax
```

## Using Printk

### Basic Usage
```c
#include "printk.h"

printk("Hello, Kernel!\n");
printk("Number: %d, Hex: 0x%x\n", 42, 0xFF);
printk("String: %s, Char: %c\n", "test", 'A');
```

### With Variables
```c
uint32_t esp = get_esp();  // Inline asm to get ESP
printk("Stack pointer: 0x%x\n", esp);
```

### Stack Information
```c
print_stack();  // Shows ESP, EBP, and stack contents
print_hex("Custom Label: ", 0xDEADBEEF);
```

## Testing GDT

### In QEMU with Serial Output
The kernel output goes to COM1 (serial console). In QEMU:
```bash
make run  # Output goes to terminal
```

### Verifying GDT Placement
Check with objdump:
```bash
i686-elf-objdump -s mykernel.bin | grep -A 10 "Contents"
```

Look for `.gdt` section at file offset matching 0x00000800.

## Troubleshooting

### Build Fails: "i686-elf-gcc not found"
→ Use `make docker-iso` instead of `make iso`

### Serial Output Not Showing
→ Make sure QEMU is run with `-serial stdio` flag
→ Kernel boot message appears in VGA buffer first (top-left shows "42")

### Stack Not Aligned Correctly
→ Already fixed in boot.asm with `align 16` directive
→ Stack grows from 0x101800 downward in 8 KB block

### GDT Not Loading
→ Check that gdt_load() is being called in kmain()
→ Verify linker.ld has `.gdt 0x00000800` section

## Architecture Details

### Descriptor Format (8 bytes)
```
Bytes 0-1:  Base address bits 0-15
Byte 2:     Base address bits 16-23
Byte 3:     Access byte (P,DPL,S,Type)
Byte 4:     Granularity (G,DB,L,Limit high)
Byte 5:     Base address bits 24-31
```

### Privilege Levels
```
Ring 0: Kernel (full access)
Ring 1: Rarely used
Ring 2: Rarely used
Ring 3: User space (restricted)
```

## Next Steps

The GDT and stack are ready for:
1. **IDT Implementation** - Interrupt Descriptor Table
2. **Exception Handling** - Page faults, divide by zero, etc.
3. **Memory Paging** - Virtual memory management
4. **User Mode** - Transition from kernel to user space
5. **System Calls** - Interface between user and kernel

## Documentation Files

For more detailed information, read:
1. `IMPLEMENTATION_SUMMARY.md` - High-level overview
2. `GDT_IMPLEMENTATION.md` - Detailed technical guide
3. `GDT_DESCRIPTOR_FORMAT.md` - Reference for GDT structure
4. `VERIFICATION_CHECKLIST.md` - Implementation verification

## Support

All code is well-commented and documented. Each function has:
- Clear purpose description
- Parameter documentation
- Return value documentation
- Usage examples in comments

Refer to the header files (*.h) for API documentation.

---

**Status**: Ready for compilation and testing ✅

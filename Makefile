# Makefile for KFS - Kernel From Scratch

# Compiler and assembler
AS = nasm
CC = gcc
LD = ld

# Flags
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -nostdlib -lgcc
LDFLAGS = -m elf_i386 -T linker.ld

# Files
BOOT_OBJ = boot.o
KERNEL_OBJ = kernel.o
OBJECTS = $(BOOT_OBJ) $(KERNEL_OBJ)
KERNEL_BIN = kfs.bin

# Default target
all: $(KERNEL_BIN)

# Build the kernel binary
$(KERNEL_BIN): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

# Assemble boot.asm
$(BOOT_OBJ): boot.asm
	$(AS) $(ASFLAGS) $< -o $@

# Compile kernel.c
$(KERNEL_OBJ): kernel.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(KERNEL_BIN)

# Run in QEMU (if available)
run: $(KERNEL_BIN)
	qemu-system-i386 -kernel $(KERNEL_BIN)

# Create ISO image for booting (requires grub-mkrescue)
iso: $(KERNEL_BIN)
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/kfs.bin
	echo 'menuentry "KFS" {' > isodir/boot/grub/grub.cfg
	echo '    multiboot /boot/kfs.bin' >> isodir/boot/grub/grub.cfg
	echo '}' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o kfs.iso isodir

.PHONY: all clean run iso

# ============================================================
#   Makefile for building a 32-bit i386 kernel using Docker
# ============================================================

# --- Correct OSDev cross-compiler toolchain ---
CC      = i686-elf-gcc
LD      = i686-elf-ld
ASM     = nasm

ASMFLAGS = -f elf32
CFLAGS   = -m32 -ffreestanding -fno-stack-protector -nostdlib -Wall -Wextra -I.
LDFLAGS  = -m elf_i386 -T linker.ld

ASM_SRC = $(filter-out gdt.asm gdt_old.asm,$(wildcard *.asm))
C_SRC   = $(wildcard *.c)
OBJ     = $(ASM_SRC:.asm=.o) $(C_SRC:.c=.o)

KERNEL = mykernel.bin
ISO    = mykernel.iso

# ============================================================
#   Native build targets (used INSIDE Docker or if toolchain installed)
# ============================================================

all: $(KERNEL)

# Explicit rule to prevent gdt.asm from creating gdt.o
gdt.o: gdt.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@if [ "$<" = "gdt.asm" ] || [ "$<" = "gdt_old.asm" ]; then \
		echo "Skipping $< (excluded from build)"; \
	else \
		$(ASM) $(ASMFLAGS) $< -o $@; \
	fi

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJ)

iso: $(KERNEL) grub.cfg
	mkdir -p iso/boot/grub
	cp $(KERNEL) iso/boot/
	cp grub.cfg iso/boot/grub/grub.cfg
	i686-elf-grub-mkrescue -o $(ISO) iso

run: iso
	qemu-system-i386 -cdrom $(ISO) -m 512 -serial stdio

clean:
	rm -rf *.o $(KERNEL) $(ISO) iso

# ============================================================
#   Docker wrapper targets (run these on the host)
# ============================================================

DOCKER_IMAGE = kfs-toolchain:latest

docker-image:
	docker buildx build --platform linux/amd64 -t $(DOCKER_IMAGE) . --load

docker-iso: docker-image
	docker run --rm -v $(PWD):/src -w /src $(DOCKER_IMAGE) \
		bash -lc "make clean && make iso CC=i686-elf-gcc LD=i686-elf-ld"

docker-run: docker-iso
	docker run --rm -it -v $(PWD):/src -w /src $(DOCKER_IMAGE) \
		bash -lc 'qemu-system-i386 -cdrom mykernel.iso -m 512 -serial stdio -display none'

# Makefile for 32-bit i386 kernel (Linux/Docker friendly)

ASM?=nasm
CC?=gcc
LD?=ld

ASMFLAGS=-f elf32
CFLAGS=-ffreestanding -fno-builtin -fno-stack-protector -nostdlib -Wall -Wextra -I.
LDFLAGS=-m elf_i386 -T linker.ld

ASM_SRC=$(wildcard *.asm)
C_SRC=$(wildcard *.c)

OBJ=$(ASM_SRC:.asm=.o) $(C_SRC:.c=.o)

all: mykernel.bin

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
iso/boot/mykernel.bin: $(OBJ) linker.ld
	@mkdir -p iso/boot/grub
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

# цель iso больше ничего не копирует
iso: iso/boot/mykernel.bin grub.cfg
    mkdir -p iso/boot/grub
    cp grub.cfg iso/boot/grub/grub.cfg
    grub-mkrescue -o mykernel.iso iso
# mykernel.bin: $(OBJ) linker.ld
# 	$(LD) $(LDFLAGS) -o $@ $(OBJ)

# iso: mykernel.bin
# 	mkdir -p iso/boot/grub
# 	cp mykernel.bin iso/boot/
# 	cp grub.cfg iso/boot/grub/
# 	grub-mkrescue -o mykernel.iso iso

run: iso
	qemu-system-i386 -cdrom mykernel.iso -m 512M -serial stdio

clean:
	rm -f *.o mykernel.bin mykernel.iso
	rm -rf iso

.PHONY: docker-image docker-bin docker-iso docker-run

DOCKER_IMAGE?=kfs-build:latest

docker-image:
	docker buildx build --platform linux/amd64 -t $(DOCKER_IMAGE) . --load

docker-bin: docker-image
	docker run --rm -v $(PWD):/src -w /src $(DOCKER_IMAGE) /bin/bash -lc "make clean && make CC=i686-linux-gnu-gcc LD=i686-linux-gnu-ld ASM=nasm ASMFLAGS='-f elf32' CFLAGS='-ffreestanding -fno-builtin -nostdlib -Wall -Wextra -I.' LDFLAGS='-m elf_i386 -T linker.ld' mykernel.bin"

docker-iso: docker-image
	docker run --rm -v $(PWD):/src -w /src $(DOCKER_IMAGE) /bin/bash -lc "make clean && make CC=i686-linux-gnu-gcc LD=i686-linux-gnu-ld ASM=nasm ASMFLAGS='-f elf32' CFLAGS='-ffreestanding -fno-builtin -nostdlib -Wall -Wextra -I.' LDFLAGS='-m elf_i386 -T linker.ld' iso"

docker-run: docker-iso
    docker run --rm -it -v $(PWD):/src -w /src $(DOCKER_IMAGE) /bin/bash -lc "timeout 10s qemu-system-i386 -cdrom mykernel.iso -m 512M -serial stdio -curses || true"

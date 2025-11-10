# Makefile for 32-bit i386 kernel (Linux/Docker friendly) # Описание назначения

ASM?=nasm # Ассемблер по умолчанию
CC?=gcc # Компилятор C по умолчанию
LD?=ld # Линковщик по умолчанию

ASMFLAGS=-f elf32 # NASM генерирует 32-битные ELF-объектники
CFLAGS=-ffreestanding -fno-builtin -fno-stack-protector -nostdlib -Wall -Wextra -I. # Флаги для ядра без стандартной библиотеки
LDFLAGS=-m elf_i386 -T linker.ld # Линковка под i386, со скриптом linker.ld
# Prefer cross toolchains on macOS to build 32-bit i386 # На macOS пробуем кросс-компиляторы
ifeq ($(shell uname),Darwin) # Если система Darwin (macOS)
  ifneq (,$(shell command -v i686-elf-gcc 2>/dev/null)) # Если доступен i686-elf-gcc
    CC := i686-elf-gcc # Использовать его как компилятор
    LD := i686-elf-ld # И соответствующий линковщик
  else ifneq (,$(shell command -v i386-elf-gcc 2>/dev/null)) # Иначе, если есть i386-elf-gcc
    CC := i386-elf-gcc # Используем его
    LD := i386-elf-ld
  else ifneq (,$(shell command -v x86_64-elf-gcc 2>/dev/null)) # Иначе, если есть x86_64-elf-gcc
    CC := x86_64-elf-gcc # Используем 64-битный кросс-компилятор
    LD := x86_64-elf-ld
    CFLAGS += -m32 # Форсируем 32-битную сборку
  endif
endif

ASM_SRC=$(wildcard *.asm) # Все .asm файлы в каталоге
C_SRC=$(wildcard *.c) # Все .c файлы в каталоге

OBJ=$(ASM_SRC:.asm=.o) $(C_SRC:.c=.o) # Список объектных файлов

all: mykernel.bin # Цель по умолчанию: собрать ядро в ELF

%.o: %.asm # Правило сборки .o из .asm
# NASM компилирует в объектник
	$(ASM) $(ASMFLAGS) $< -o $@ 

%.o: %.c # Правило сборки .o из .c
# Компиляция C в объектник
	$(CC) $(CFLAGS) -c $< -o $@ 
	
iso/boot/mykernel.bin: $(OBJ) linker.ld # Линкуем ядро прямо в ISO-дерево
	@mkdir -p iso/boot/grub # Создаём каталоги для GRUB
	$(LD) $(LDFLAGS) -o $@ $(OBJ) # Собираем ELF бинарь ядра

# цель iso больше ничего не копирует # Сборка ISO-образа с GRUB
iso: iso/boot/mykernel.bin grub.cfg # Требует собранного ядра и конфигурации GRUB
# Убедиться, что каталог есть
	mkdir -p iso/boot/grub 
# Поместить grub.cfg в ISO-дерево
	cp grub.cfg iso/boot/grub/grub.cfg 
# Создать ISO из папки iso/
	@if command -v grub-mkrescue >/dev/null 2>&1; then \
		echo "==> Using local grub-mkrescue"; \
		grub-mkrescue -o mykernel.iso iso; \
	elif command -v grub2-mkrescue >/dev/null 2>&1; then \
		echo "==> Using local grub2-mkrescue"; \
		grub2-mkrescue -o mykernel.iso iso; \
	else \
		echo "==> grub-mkrescue не найден локально. Перехожу на Docker фолбэк..."; \
		$(MAKE) docker-iso; \
	fi 
# Standalone kernel binary (no ISO) # Сборка ядра без ISO
mykernel.bin: $(OBJ) linker.ld
# Линковка ELF ядра
	$(LD) $(LDFLAGS) -o $@ $(OBJ) 

# iso: mykernel.bin # Альтернативный сценарий копирования бинаря в ISO
# 	mkdir -p iso/boot/grub
# 	cp mykernel.bin iso/boot/
# 	cp grub.cfg iso/boot/grub/
# 	grub-mkrescue -o mykernel.iso iso

run: iso # Запустить через GRUB/ISO в QEMU
# Сериал перенаправлен в терминал
	qemu-system-i386 -cdrom mykernel.iso -m 512M -serial stdio

# Direct boot without GRUB/ISO (Multiboot kernel) # Прямой запуск ядра без ISO
run-kernel: mykernel.bin
# Быстрее, без GRUB
	qemu-system-i386 -kernel mykernel.bin -m 512M -serial stdio 

 

 
clean: # Очистить артефакты сборки
# Удалить объектники и образы
	rm -f *.o mykernel.bin mykernel.iso
	rm -rf iso # Удалить дерево ISO
 
# Явные phony-цели
.PHONY: docker-image docker-bin docker-iso docker-run 

DOCKER_IMAGE?=kfs-build:latest # Имя локального docker-образа

docker-image: # Собрать docker-образ с тулчейном
	docker buildx build --platform linux/amd64 -t $(DOCKER_IMAGE) . --load # Гарантируем amd64

docker-bin: docker-image # Собрать ядро внутри контейнера
# Жёстко фиксируем тулчейн
	docker run --rm -v $(PWD):/src -w /src $(DOCKER_IMAGE) /bin/bash -lc "make clean && make CC=i686-linux-gnu-gcc LD=i686-linux-gnu-ld ASM=nasm ASMFLAGS='-f elf32' CFLAGS='-ffreestanding -fno-builtin -nostdlib -Wall -Wextra -I.' LDFLAGS='-m elf_i386 -T linker.ld' mykernel.bin" 

docker-iso: docker-image # Собрать ISO в контейнере
# Собрать ISO-образ
	docker run --rm -v $(PWD):/src -w /src $(DOCKER_IMAGE) /bin/bash -lc "make clean && make CC=i686-linux-gnu-gcc LD=i686-linux-gnu-ld ASM=nasm ASMFLAGS='-f elf32' CFLAGS='-ffreestanding -fno-builtin -nostdlib -Wall -Wextra -I.' LDFLAGS='-m elf_i386 -T linker.ld' iso" 

docker-run: docker-iso # Запустить QEMU с ISO в контейнере
# Без GUI, только сериал
	docker run --rm -it -v $(PWD):/src -w /src $(DOCKER_IMAGE) /bin/bash -lc "timeout 10s qemu-system-i386 -cdrom mykernel.iso -m 512M -serial stdio -display none || true" 

# Docker: run kernel directly (no GRUB/ISO), serial only # Прямой запуск ядра в контейнере
docker-run-kernel: docker-bin
# Быстрый тест
	docker run --rm -it -v $(PWD):/src -w /src $(DOCKER_IMAGE) /bin/bash -lc "timeout 10s qemu-system-i386 -kernel mykernel.bin -m 512M -serial stdio -display none || true" 


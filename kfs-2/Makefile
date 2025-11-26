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

run: iso # Запустить через GRUB/ISO в QEMU
# Сериал перенаправлен в терминал
	qemu-system-i386 -cdrom mykernel.iso -m 512M -serial stdio
 
clean: # Очистить артефакты сборки
# Удалить объектники и образы
	rm -f *.o mykernel.bin mykernel.iso
	rm -rf iso # Удалить дерево ISO
 
# Явные phony-цели
.PHONY: docker-image docker-bin docker-iso docker-run 

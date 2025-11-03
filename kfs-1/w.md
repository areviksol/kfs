## A kernel you can boot via GRUB 

* **Файлы**: `kfs-1/mykernel.bin, kfs-1/grub.cfg, kfs-1/iso/boot/grub/grub.cfg`
* **Сборка/запуск**: `цель iso и run в kfs-1/Makefile (также есть docker-iso, docker-run)`

## ASM bootable base 

* **Файл**: `kfs-1/boot.asm (Multiboot v1 header, точка входа start, вызов kmain)`

## A basic kernel library, with basics functions and types

* **Файлы**: `kfs-1/lib.c, kfs-1/lib.h (strlen, strcmp, memset, memcpy), kfs-1/types.h (size_t, <stdint.h>)`

## Some basic code to print some stuff on the screen

* **Файл:** `kfs-1/kernel.c (запись в VGA по адресу 0xB8000, вывод символов '4' и '2')`
* **Дополнительно**: `вывод "42" в последовательный порт (COM1)`

## A basic "Hello world" kernel

`Сейчас выводится число "42" (на экран и в serial). Для строгого «Hello, world!» нужно заменить вывод на строку.`

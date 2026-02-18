; gdt.asm -- defines a GDT placed in section .gdt (we'll map that to 0x00000800 in linker.ld)
; NASM syntax; compiled by your Makefile (ASM=nasm -f elf32)
BITS 32

SECTION .gdt ALIGN=16

; helper macro: GDT_ENTRY base, limit, access, flags
%macro GDT_ENTRY 4
  dw %2 & 0xFFFF
  dw %1 & 0xFFFF
  db (%1 >> 16) & 0xFF
  db %3 & 0xFF
  db (((%2 >> 16) & 0x0F) | ((%4 & 0x0F) << 4)) & 0xFF
  db (%1 >> 24) & 0xFF
%endmacro

gdt_start:
  dq 0x0000000000000000   ; null descriptor

  ; Kernel code: base=0, limit=0xFFFFF, access=0x9A, flags=0x0C (granularity=4KB, 32-bit)
  GDT_ENTRY 0x00000000, 0x000FFFFF, 0x9A, 0x0C

  ; Kernel data: access=0x92
  GDT_ENTRY 0x00000000, 0x000FFFFF, 0x92, 0x0C

  ; Kernel stack: data segment (we make a distinct selector for clarity) access=0x92
  GDT_ENTRY 0x00000000, 0x000FFFFF, 0x92, 0x0C

  ; User code: DPL=3 (0xFA)
  GDT_ENTRY 0x00000000, 0x000FFFFF, 0xFA, 0x0C

  ; User data: DPL=3 (0xF2)
  GDT_ENTRY 0x00000000, 0x000FFFFF, 0xF2, 0x0C

  ; User stack: DPL=3 (0xF2)
  GDT_ENTRY 0x00000000, 0x000FFFFF, 0xF2, 0x0C

gdt_end:

; GDTR structure: 2 bytes limit, 4 bytes base
gdtr:
  dw gdt_end - gdt_start - 1
  dd gdt_start

SECTION .text
global gdt_init

gdt_init:
  cli
  lgdt [gdtr]

  ; Reload cs by far jump to kernel code selector (index 1 -> selector 0x08)
  ; Use a far jump to load CS properly.
  jmp 0x08:reload_cs

reload_cs:
  ; Set data segment registers to kernel data selector (index 2 -> selector 0x10)
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  ; NOTE: We do not change SS/ESP here; kernel C will set SS and ESP to the kernel stack.
  sti
  ret

BITS 32

section .multiboot
align 4
    ; Multiboot v1 header (simple)
    dd 0x1BADB002        ; magic
    dd 0x0               ; flags = 0
    dd -(0x1BADB002 + 0x0) ; checksum so (magic+flags+checksum) == 0

section .text
global start
extern kmain

start:
    cli                     ; disable interrupts while we setup stack
    ; initialize a stack in .bss (stack_end label defined in .bss)
    mov esp, stack_end
    call kmain

.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_space:    resb 8192   ; 8 KB stack
stack_end:

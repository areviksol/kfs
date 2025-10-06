; boot.asm - Multiboot compliant bootloader
; This bootloader sets up the system and calls the kernel

section .multiboot
    ; Multiboot header
    MULTIBOOT_MAGIC     equ 0x1BADB002
    MULTIBOOT_FLAGS     equ 0x0
    MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
    
    align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

section .bss
    align 16
    stack_bottom:
        resb 16384  ; 16 KiB stack
    stack_top:

section .text
    global _start
    extern kernel_main
    
_start:
    ; Set up the stack
    mov esp, stack_top
    
    ; Call the kernel main function
    call kernel_main
    
    ; Hang if kernel_main returns
    cli             ; Clear interrupts
.hang:
    hlt             ; Halt the CPU
    jmp .hang       ; Jump back if wake up

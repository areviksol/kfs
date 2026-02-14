; idt_load.asm - Load IDT register and define interrupt handlers

section .text
global idt_load_register
global isr0_handler
global isr8_handler
global irq0_handler
global irq1_handler

extern keyboard_irq_handler

; Load IDT Register (LIDT instruction)
; Parameters: edi = pointer to IDTR structure (on 32-bit, first arg is on stack)
idt_load_register:
    mov eax, [esp + 4]          ; Get IDTR pointer from stack (32-bit calling convention)
    lidt [eax]                  ; Load IDTR
    ret

; ISR0: Division by Zero (Exception)
isr0_handler:
    push byte 0                 ; Error code (no error code, so push 0)
    push byte 0                 ; ISR number
    jmp isr_common_handler

; ISR8: Double Fault (Exception)
isr8_handler:
    ; Double fault has an error code
    push byte 8                 ; ISR number
    jmp isr_common_handler

; IRQ0: Timer (Programmable Interval Timer)
irq0_handler:
    push byte 0                 ; No error code
    push byte 32                ; IRQ0 = ISR 32
    jmp irq_common_handler

; IRQ1: Keyboard (PS/2)
irq1_handler:
    push byte 0                 ; No error code
    push byte 33                ; IRQ1 = ISR 33
    jmp irq_common_handler

; Common ISR handler
isr_common_handler:
    pusha                       ; Push all general-purpose registers
    mov eax, [esp + 32]         ; Get ISR number from stack
    mov ebx, [esp + 36]         ; Get error code from stack
    
    ; For now, just acknowledge and return
    ; Later: call handler function with eax (ISR number) and ebx (error code)
    
    popa
    add esp, 8                  ; Remove ISR number and error code
    iret

; Common IRQ handler
irq_common_handler:
    pusha                       ; Push all general-purpose registers
    
    mov eax, [esp + 32]         ; Get IRQ number from stack
    
    ; Check if it's IRQ1 (keyboard)
    cmp al, 33
    je handle_keyboard
    
    ; For other IRQs, just acknowledge PIC and return
    mov al, 0x20                ; End of Interrupt (EOI) command
    out 0x20, al                ; Send EOI to master PIC
    
    popa
    add esp, 8
    iret

handle_keyboard:
    ; Read scancode from keyboard port
    mov edx, 0x60               ; Keyboard data port
    in al, dx                   ; Read scancode into AL
    
    ; Save the scancode
    mov ecx, eax                ; Save scancode in ECX
    
    ; Restore general registers before calling C function
    popa
    
    ; Push scancode as argument (32-bit cdecl calling convention)
    push ecx                    ; Push scancode as first argument
    call keyboard_irq_handler   ; Call the C function
    add esp, 4                  ; Clean up the argument
    
    ; Send EOI to PIC
    mov al, 0x20
    out 0x20, al                ; Master PIC
    mov al, 0x20
    out 0xA0, al                ; Slave PIC (in case)
    
    add esp, 8                  ; Remove IRQ number and error code
    iret

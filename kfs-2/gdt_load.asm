; gdt.asm - GDT loading and stack management routines

BITS 32

section .text
    global gdt_load

; void gdt_load(struct gdtr *gdtr)
; Load the GDT register and update segment selectors
; Argument passed via ESP+4 (cdecl calling convention)
gdt_load:
    push ebp
    mov ebp, esp
    
    mov eax, [ebp + 8]      ; EAX = pointer to GDTR structure
    lgdt [eax]              ; Load the GDT register
    
    ; Reload segment selectors
    ; We use a far jump to reload CS (code segment)
    jmp 0x08:.flush         ; Far jump with kernel code selector (0x08)
    
.flush:
    ; Reload data segment selectors (DS, SS, ES, FS, GS)
    mov eax, 0x10           ; Kernel data selector (0x10)
    mov ds, eax
    mov ss, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    
    pop ebp
    ret

; void get_stack_pointer(uint32_t *esp_ptr)
; Get the current stack pointer value
; This is used by printk to display the stack
global get_stack_pointer
get_stack_pointer:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]      ; EAX = pointer to uint32_t
    mov ecx, [ebp + 4]      ; ECX = return address (ESP before this function)
    mov [eax], ecx          ; Store the old ESP into the provided pointer
    pop ebp
    ret

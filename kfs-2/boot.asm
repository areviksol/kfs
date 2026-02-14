BITS 32 ; 32-битный код (GRUB уже перевёл процессор в protected mode)

section .multiboot ; Секция заголовка Multiboot v1
align 4            ; Выравнивание заголовка на 4 байта
    dd 0x1BADB002  ; magic — сигнатура Multiboot
    dd 0x0         ; flags = 0 (никаких дополнительных требований)
    dd -(0x1BADB002 + 0x0) ; checksum: magic + flags + checksum == 0

section .text      ; Кодовая секция
global start       ; Экспорт точки входа для линковщика
extern kmain       ; Внешняя C-функция (определена в kernel.c)

start:             ; Точка входа ядра
    cli           ; Отключить прерывания на время начальной инициализации
    mov esp, stack_end ; Инициализировать указатель стека вершиной заранее выделенной области
    ; Multiboot v1: при входе EAX=magic (0x2BADB002), EBX=адрес multiboot_info
    ; Передадим их в kmain по cdecl через стек: сначала EBX, затем EAX
    push ebx
    push eax
    call kmain     ; Передать управление в C-ядро: kmain(uint32_t magic, uint32_t mbi_addr)
    add esp, 8     ; Очистить стек (2 аргумента по 4 байта)

.hang:             ; Резервный бесконечный цикл на случай возврата из kmain
    hlt            ; Остановить CPU до следующего события/прерывания
    jmp .hang      ; Зациклиться навсегда

section .bss       ; Неинициализированные данные
align 16           ; Выравнивание стека по 16 байт (ABI-дружелюбно)
stack_space:    resb 8192  ; 8 КБ под стек
stack_end:           ; Метка вершины стека (адрес для ESP)

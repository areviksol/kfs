BITS 32 ; Используем 32-битный режим (GRUB уже переводит в protected mode)

section .multiboot ; Секция заголовка Multiboot v1
align 4 ; Выравнивание по 4 байта
    ; Multiboot v1 header (simple)
    dd 0x1BADB002        ; magic — сигнатура Multiboot
    dd 0x0               ; flags = 0 — без особых требований
    dd -(0x1BADB002 + 0x0) ; checksum: magic + flags + checksum == 0

section .text ; Кодовая секция
global start ; Точка входа, объявляется для линковщика
extern kmain ; Внешняя функция на C, определена в kernel.c

start: ; Входная точка ядра
    cli                     ; Отключить прерывания во время настройки стека
    ; initialize a stack in .bss (stack_end label defined in .bss)
    mov esp, stack_end      ; Установить вершину стека
    call kmain              ; Передать управление C-коду

.hang: ; Бесконечный цикл «ожидания» на случай возврата из kmain
    hlt ; Остановка до следующего события
    jmp .hang ; Зациклить

section .bss ; Неинициализированные данные
align 16 ; Выравнивание стека по 16 байт
stack_space:    resb 8192   ; 8 KB stack
stack_end: ; Метка конца области — вершина стека

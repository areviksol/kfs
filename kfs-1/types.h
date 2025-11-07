#ifndef TYPES_H // Защита от повторного включения
#define TYPES_H

#include <stdint.h>  // Предоставляет uint8_t, uint16_t, uint32_t, uint64_t и т. п.

// Определяем size_t только если компилятор не предоставил его сам
#ifndef __SIZE_TYPE__
typedef uint32_t size_t;  // Для 32-битного ядра size_t — 32-битное беззнаковое целое
#endif

#endif // TYPES_H

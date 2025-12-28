#ifndef TYPES_H // Защита от повторного включения
#define TYPES_H

#include <stdint.h>  // Предоставляет uint8_t, uint16_t, uint32_t, uint64_t и т. п.

// Определяем size_t только если компилятор не предоставил его сам
#ifndef __cplusplus
typedef uint32_t size_t;
#endif

#endif // TYPES_H

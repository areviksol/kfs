#ifndef LIB_H // Защита от повторного включения заголовка
#define LIB_H

#include <stddef.h> // Для определения size_t

size_t strlen(const char *str); // Возвращает длину строки до символа '\0'
int strcmp(const char *a, const char *b); // Сравнивает две строки, возвращает разницу
void *memset(void *dest, int val, size_t len); // Заполняет len байт по адресу dest значением val
void *memcpy(void *dest, const void *src, size_t len); // Копирует len байт из src в dest

#endif // LIB_H

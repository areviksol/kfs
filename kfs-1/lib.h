#ifndef LIB_H
#define LIB_H

#include <stddef.h>

size_t strlen(const char *str);
int strcmp(const char *a, const char *b);
void *memset(void *dest, int val, size_t len);
void *memcpy(void *dest, const void *src, size_t len);

#endif

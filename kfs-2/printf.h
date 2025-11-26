#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>
#include <stddef.h>

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
int snprintf(char *buf, size_t size, const char *fmt, ...);

/* printk: printf-style kernel print (writes to VGA and serial) */
int printk(const char *fmt, ...);

#endif /* PRINTF_H */

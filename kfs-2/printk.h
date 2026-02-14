#ifndef PRINTK_H
#define PRINTK_H

#include <stdint.h>
#include <stdarg.h>

/* Print formatted output to serial console */
void printk(const char *fmt, ...);

/* Print kernel stack information in human-friendly format */
void print_stack(void);

/* Print a single hex value with label */
void print_hex(const char *label, uint32_t value);

#endif /* PRINTK_H */

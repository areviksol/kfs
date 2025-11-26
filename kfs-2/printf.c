/* Minimal vsnprintf/snprintf for kernel use.
 * Supports: %s, %c, %d, %u, %x, %%.
 * Not locale-aware, tiny and safe for freestanding kernels.
 */

 #include "printf.h"
 #include "lib.h"   /* for strlen */
 #include <stdarg.h>
 #include <stddef.h>
 
 static void out_char(char **buf, size_t *rem, char c) {
     if (*rem > 1) {
         **buf = c;
         (*buf)++;
         (*rem)--;
     } else {
         /* when no room, still advance pointer to count length */
         (*buf)++;
     }
 }
 
 static void out_str(char **buf, size_t *rem, const char *s) {
     while (*s) out_char(buf, rem, *s++);
 }
 
 static void out_uint_base(char **buf, size_t *rem, unsigned int v, int base, int width, int pad_zero) {
     char tmp[32];
     int pos = 0;
     if (v == 0) {
         tmp[pos++] = '0';
     } else {
         while (v && pos < (int)sizeof(tmp)) {
             unsigned int d = v % base;
             tmp[pos++] = (d < 10) ? ('0' + d) : ('a' + d - 10);
             v /= base;
         }
     }
     while (pos < width) { out_char(buf, rem, pad_zero ? '0' : ' '); width--; }
     while (pos--) out_char(buf, rem, tmp[pos]);
 }
 
 int vsnprintf(char *buf, size_t size, const char *fmt, va_list args) {
     char *out = buf;
     size_t rem = size;
     const char *p = fmt;
 
     if (rem == 0) return 0; /* nothing to do */
 
     while (*p) {
         if (*p != '%') {
             out_char(&out, &rem, *p++);
             continue;
         }
         p++; /* skip '%' */
 
         /* simple width/zero parsing (e.g. %02d) */
         int width = 0;
         int pad_zero = 0;
         if (*p == '0') { pad_zero = 1; p++; }
         while (*p >= '0' && *p <= '9') {
             width = width * 10 + (*p - '0');
             p++;
         }
 
         switch (*p) {
             case 'c': {
                 int c = va_arg(args, int);
                 out_char(&out, &rem, (char)c);
                 break;
             }
             case 's': {
                 const char *s = va_arg(args, const char*);
                 if (!s) s = "(null)";
                 out_str(&out, &rem, s);
                 break;
             }
             case 'd': {
                 int v = va_arg(args, int);
                 if (v < 0) {
                     out_char(&out, &rem, '-');
                     out_uint_base(&out, &rem, (unsigned int)(-v), 10, width, pad_zero);
                 } else {
                     out_uint_base(&out, &rem, (unsigned int)v, 10, width, pad_zero);
                 }
                 break;
             }
             case 'u': {
                 unsigned int v = va_arg(args, unsigned int);
                 out_uint_base(&out, &rem, v, 10, width, pad_zero);
                 break;
             }
             case 'x': {
                 unsigned int v = va_arg(args, unsigned int);
                 out_uint_base(&out, &rem, v, 16, width, pad_zero);
                 break;
             }
             case '%': {
                 out_char(&out, &rem, '%');
                 break;
             }
             default:
                 out_char(&out, &rem, '?');
                 break;
         }
         p++;
     }
 
     if (rem > 0) {
         *out = '\0';
     } else {
         /* ensure NUL at last possible position */
         buf[size - 1] = '\0';
     }
 
     return (int)(out - buf);
 }
 
 int snprintf(char *buf, size_t size, const char *fmt, ...) {
     va_list ap;
     va_start(ap, fmt);
     int r = vsnprintf(buf, size, fmt, ap);
     va_end(ap);
     return r;
 }
 
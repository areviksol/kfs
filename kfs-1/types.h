#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>  // provides uint8_t, uint16_t, uint32_t, uint64_t, etc.

// Only define size_t for your kernel if not already defined
#ifndef __SIZE_TYPE__
typedef uint32_t size_t;  // for 32-bit kernel
#endif

#endif

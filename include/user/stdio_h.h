#ifndef __USER_STDIO_H
#define __USER_STDIO_H

#include "sys/std_int_h.h"

typedef char* va_list;
uint32_t printf(const char* str, ...);
uint32_t vsprintf(char* str, const char* format, va_list ap);
uint32_t printk(const char* format, ...);
#endif

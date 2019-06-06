#ifndef __KERNEL_STRING_H
#define __KERNEL_STRING_H
void memset(void * dest_, uint8_t value, uint32_t size);
char* strcpy(char* dst_, const char* src_);
void memcpy(void* dst_, const void* src_, uint32_t size);
#endif

#ifndef __KERNEL_STRING_H
#define __KERNEL_STRING_H
void memset(void * dest_, uint8_t value, uint32_t size);
char* strcpy(char* dst_, const char* src_);
void memcpy(void* dst_, const void* src_, uint32_t size);
uint32_t strlen(const char* str);
int8_t strcmp (const char *a, const char *b);
char* strcat(char* dst_, const char* src_);
char* strrchr(const char* str, const uint8_t ch) ;
#endif

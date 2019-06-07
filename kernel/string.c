#include "kernel/debug_h.h"
#include "sys/std_int_h.h"

//  dst_ 起始size个字节置为value
void memset(void * dest_, uint8_t value, uint32_t size){
    ASSERT(dest_ != NULL);
    uint8_t* dst = (uint8_t*)dest_;
    while(size -- > 0){
        *dst++ = value;
    }
}



// 拷贝字符串
char* strcpy(char* dst_, const char* src_) {
   ASSERT(dst_ != NULL && src_ != NULL);
   char* r = dst_;		       // 用来返回目的字符串起始地址
   while((*dst_++ = *src_++));
   return r;
}



// 将src_起始的size个字节复制到dst_
void memcpy(void* dst_, const void* src_, uint32_t size) {
   ASSERT(dst_ != NULL && src_ != NULL);
   uint8_t* dst = dst_;
   const uint8_t* src = src_;
   while (size-- > 0)
      *dst++ = *src++;
}

// 返回字符串长度
uint32_t strlen(const char* str) {
   ASSERT(str != NULL);
   const char* p = str;
   while(*p++);
   return (p - str - 1);
}

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


// 比较两个字符串,若a_中的字符大于b_中的字符返回1,相等时返回0,否则返回-1.
int8_t strcmp (const char* a, const char* b) {
    ASSERT(a != NULL && b != NULL);
    while (*a != 0 && *a == *b) {
        a++;
        b++;
    }
    /* 如果*a小于*b就返回-1,否则就属于*a大于等于*b的情况。在后面的布尔表达式"*a > *b"中,
    * 若*a大于*b,表达式就等于1,否则就表达式不成立,也就是布尔值为0,恰恰表示*a等于*b */
    return *a < *b ? -1 : *a > *b;
}

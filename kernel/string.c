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


//将字符串src_拼接到dst_后,将回拼接的串地址
char* strcat(char* dst_, const char* src_) {
   ASSERT(dst_ != NULL && src_ != NULL);
   char* str = dst_;
   while (*str++);
   --str;      // 别看错了，--str是独立的一句，并不是while的循环体
   while((*str++ = *src_++));	 // 当*str被赋值为0时,此时表达式不成立,正好添加了字符串结尾的0.
   return dst_;
}


//从后往前查找字符串str中首次出现字符ch的地址(不是下标,是地址)
char* strrchr(const char* str, const uint8_t ch) {
   ASSERT(str != NULL);
   const char* last_char = NULL;
   /* 从头到尾遍历一次,若存在ch字符,last_char总是该字符最后一次出现在串中的地址(不是下标,是地址)*/
   while (*str != 0) {
      if (*str == ch) {
	 last_char = str;
      }
      str++;
   }
   return (char*)last_char;
}

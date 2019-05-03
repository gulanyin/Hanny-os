/* 将字符串从src_复制到dst_ */
#include "debug_h.h"

char* strcpy(char* dst_, const char* src_) {
   ASSERT(dst_ != NULL && src_ != NULL);
   char* r = dst_;		       // 用来返回目的字符串起始地址
   while((*dst_++ = *src_++));
   return r;
}

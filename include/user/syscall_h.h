#ifndef __USER_SYSCALL_H
#define __USER_SYSCALL_H

#include "sys/std_int_h.h"

void init_all_syscall();
uint32_t getpid();
uint32_t write(char* str) ;
#endif

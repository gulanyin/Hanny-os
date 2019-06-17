#ifndef __KERNEL_PRINT_H
#define __KERNEL_PRINT_H

// 定义在kerner/print_s.asm汇编文件中的打印函数

void print_char(unsigned char);
void print_str(unsigned char*);
void print_int_oct(unsigned int);
void cls_screen();

#endif

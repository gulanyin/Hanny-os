#ifndef __KERNEL_INTERRUPT_H
#define __KERNEL_INTERRUPT_H

// interrupt_s.asm 汇编中提供的中断相关函数

// disalbe_interrupt:
void interrupt_off();  // 关中断

void interrupt_on();  // 开中断

#endif /*__KERNEL_INTERRUPT_H*/

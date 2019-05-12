#ifndef __KERNEL_INTERRUPT_H
#define __KERNEL_INTERRUPT_H




/* 定义中断的两种状态:
 * INTERRUPT_OFF值为0,表示关中断,
 * INTERRUPT_ON值为1,表示开中断 */
enum interrupt_status {		 // 中断状态
    INTERRUPT_OFF,			 // 中断关闭
    INTERRUPT_ON		         // 中断打开
};


enum interrupt_status get_interrupt_status();
enum interrupt_status interrupt_enable();
enum interrupt_status interrupt_disable();
enum interrupt_status set_interrupt_status(enum interrupt_status status);
void setup_interrupt();
void entry_init_interrupt();
#endif


static inline void on_interrupt(){
    asm volatile ( "sti");
}

static inline void off_interrupt(){
    asm volatile ( "cli");
}

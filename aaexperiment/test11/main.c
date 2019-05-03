#include "debug_h.h"
#include "print_h.h"
#include "thread_h.h"
#include "interrupt_h.h"



void main(){
    print_char('M');
    print_str("main function\n");

    // 设置中断8259A,时钟频率
    init_interrupt();
    // 注册所有中断函数
    register_int_all();


    print_str("init interrupt ok!\n");

}

#include "kernel/print_h.h"
#include "mm/init_memory_h.h"
#include "kernel/interrupt_h.h"


void main(){

    // 内存管理
    init_memory();
    // // test
    // 分配的第一个内核内存页，虚拟起始地址应该为 0xc000 0000 + 0x0020 2000  = > 0xc020 2000
    // uint32_t one_test_page = (uint32_t)get_kernel_pages(1);
    // print_str("\none_test_page virtual_addr_start:");print_int_oct(one_test_page);
    // while(1);


    // 中断设置
    init_interrupt();

    while (1) {
        /* code */
    }


}

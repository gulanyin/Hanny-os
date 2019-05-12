#include "kernel/print_h.h"
#include "mm/init_memory_h.h"
#include "kernel/interrupt_h.h"
#include "thread/thread_h.h"
#include "device/console_h.h"


void k_thread_a(void*);
void k_thread_b(void*);

void main(){

    // // 测试esp
    // uint32_t esp;
    // asm ("mov %%esp, %0" : "=g" (esp));
    // print_str("\n cccc esp:");print_int_oct(esp);

    // 内存管理
    entry_init_memory();
    // // test
    // 分配的第一个内核内存页，虚拟起始地址应该为 0xc000 0000 + 0x0020 2000  = > 0xc020 2000
    // uint32_t one_test_page = (uint32_t)get_kernel_pages(1);
    // print_str("\none_test_page virtual_addr_start:");print_int_oct(one_test_page);
    // while(1);


    // 线程管理
    entry_init_thread();
    thread_start("k_thread_a", 30, k_thread_a, "argA ");
    thread_start("k_thread_b", 10, k_thread_b, "argB ");



    // 中断设置
    entry_init_interrupt();


    while (1) {
        /* code */
        // print_str("main ");
        console_str("main ");
        asm volatile("hlt");
    }


}



// k_thread_a 函数
void k_thread_a(void* args) {
   while(1) {
      //print_str(args);
      console_str(args);
      asm volatile("hlt");
   }
}

// k_thread_b 函数
void k_thread_b(void* args) {
    while(1) {
       //print_str(args);
       console_str(args);
       asm volatile("hlt");
    }
}

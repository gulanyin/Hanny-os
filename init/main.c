#include "kernel/print_h.h"
#include "mm/init_memory_h.h"
#include "kernel/interrupt_h.h"
#include "thread/thread_h.h"
#include "device/console_h.h"
#include "userprog/tss_h.h"
#include "userprog/process_h.h"


void k_thread_a(void*);
void k_thread_b(void*);


void u_prog_a(void);
void u_prog_b(void);
int test_var_a = 1, test_var_b = 1;


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

    // 初始化控制带锁
    entyr_console_init();


    thread_start("k_thread_a", 30, k_thread_a, "argA ");
    thread_start("k_thread_b", 10, k_thread_b, "argB ");



    // 初始化tss段
    entry_init_tss();




    // 中断设置
    entry_init_interrupt();

    console_str("entry_init_interrupt ok\n ");


    // 添加两个线程结构体
    process_execute(u_prog_a, "user_prog_a");
    process_execute(u_prog_b, "user_prog_b");
    console_str("process_execute ok\n ");


    //swhile(1);
    // 开启中断
    on_interrupt();


    while (1) {
        /* code */
         //print_str("main ");
         console_str("main22 ");
        asm volatile("hlt");
    }


}



// k_thread_a 函数
void k_thread_a(void* args) {
   while(1) {
      //print_str(args);
      //console_str(args);
      console_str("test_var_a");
      console_int_oct(test_var_a);
      asm volatile("hlt");
   }
}

// k_thread_b 函数
void k_thread_b(void* args) {
    while(1) {
       //print_str(args);
       //console_str(args);
       console_str("test_var_b");
       console_int_oct(test_var_b);
       asm volatile("hlt");
    }
}



// 测试用户进程
void u_prog_a(void) {
   while(1) {
      test_var_a++;
   }
}

// 测试用户进程
void u_prog_b(void) {
   while(1) {
      test_var_b++;
   }
}

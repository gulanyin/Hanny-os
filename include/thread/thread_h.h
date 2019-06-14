#ifndef __THREAD_THEAD_H
#define __THREAD_THEAD_H

#include "sys/gloab_h.h"
#include "sys/std_int_h.h"
#include "kernel/list_h.h"
#include "mm/memory_h.h"
#include "mm/mem_desc_h.h"

typedef void (*thread_func)(void*);

// 进程或线程的状态
enum task_status {
   TASK_RUNNING,
   TASK_READY,
   TASK_BLOCKED,
   TASK_WAITING,
   TASK_HANGING,
   TASK_DIED
};


// 中断时压入的参数
struct intr_stack {
    uint32_t vec_no;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    // 以下由cpu从低特权级进入高特权级时压入
    uint32_t err_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
};


// 线程栈
struct thread_stack {
   uint32_t ebp;
   uint32_t ebx;
   uint32_t edi;
   uint32_t esi;

   //线程第一次执行时,eip指向待调用的函数kernel_thread, 其它时候,eip是指向switch_to的返回地址
   void (*eip) (thread_func func, void* func_arg);

    // 参数unused_ret只为占位置充数为返回地址
    uint32_t unused_retaddr;
    // 线程函数
    thread_func function;
    // 线程函数所需的参数
    void* func_arg;
};



// 进程或线程的pcb,程序控制块
struct task_struct {
   uint32_t* self_kstack;	 // 各内核线程都用自己的内核栈
   enum task_status status;
   char name[16];
   uint8_t priority;
   uint8_t ticks;	   // 每次在处理器上执行的时间嘀嗒数

   //此任务自上cpu运行后至今占用了多少cpu嘀嗒数,也就是此任务执行了多久
   uint32_t elapsed_ticks;

   uint32_t pid;

   // general_tag的作用是用于线程在一般的队列中的结点
   LIST_ELEM general_tag;

   // all_list_tag的作用是用于线程队列thread_all_list中的结点
   LIST_ELEM all_list_tag;

   uint32_t* pgdir;              // 进程自己页表的虚拟地址

   VIRTUAL_ADDRESS userprog_vaddr;  // 用户进程的虚拟地址
   struct mem_block_desc u_block_desc[DESC_COUNT];

   int32_t fd_table[8]; // 文件描述符下标数组，数组元素为全局文件描述符列表中的下标

   uint32_t stack_magic;	 // 用这串数字做栈的边界标记,用于检测栈的溢出
};






void schedule();
struct task_struct* thread_start(char* name, int prio, thread_func function, void* func_arg);
void entry_init_thread();
struct task_struct* running_thread();

void thread_block(enum task_status status);
void thread_unblock(struct task_struct* pthread);
void init_thread(struct task_struct* pthread, char* name, int prio);
void thread_create(struct task_struct* pthread, thread_func function, void* func_arg) ;
void thread_yield(void);

#endif

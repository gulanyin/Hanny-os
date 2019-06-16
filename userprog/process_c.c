#include "thread/thread_h.h"
#include "kernel/list_h.h"
#include "kernel/interrupt_h.h"
#include "kernel/debug_h.h"
#include "kernel/string_h.h"
#include "mm/init_memory_h.h"
#include "kernel/bitmap_h.h"
#include "device/console_h.h"
#include "userprog/tss_h.h"
#include "mm/mem_desc_h.h"

#define DIV_ROUND_UP(X, STEP) ((X + STEP - 1) / (STEP))

#define PG_SIZE 4096
#define EFLAGS_MBS	(1 << 1)	// 此项必须要设置
#define EFLAGS_IF_1	(1 << 9)	// if为1,开中断
#define EFLAGS_IF_0	0		// if为0,关中断
#define EFLAGS_IOPL_3	(3 << 12)	// IOPL3,用于测试用户程序在非系统调用下进行IO
#define EFLAGS_IOPL_0	(0 << 12)	// IOPL0
#define PF_USER 2
#define USER_STACK3_VADDR (0xc0000000 - 0x1000)

extern void interrupt_exit(void);

extern LIST thread_ready_list;	            // 就绪队列
extern LIST thread_all_list;	            // 所有任务队列

void start_process(void* filename_){
    void* function = filename_;
    struct task_struct* cur = running_thread();

    cur->self_kstack += sizeof(struct thread_stack);
    struct intr_stack* proc_stack = (struct intr_stack*)cur->self_kstack;

    proc_stack->edi = 0;
    proc_stack->esi = 0;
    proc_stack->ebp = 0;
    proc_stack->esp_dummy = 0;

    proc_stack->ebx = 0;
    proc_stack->edx = 0;
    proc_stack->ecx = 0;
    proc_stack->eax = 0;

    proc_stack->gs = 0;
    proc_stack->fs = 0x33;
    proc_stack->es = 0x33;
    proc_stack->ds = 0x33;

    // 0x33 第六个选择子，数据堆栈段
    // 0x2b 第5个选择子，代码段
    proc_stack->eip = (uint32_t)function;	 // 待执行的用户程序地址
    proc_stack->cs = 0x2b;	 // 待执行的用户程序地址

    proc_stack->eflags = (EFLAGS_IOPL_0 | EFLAGS_MBS | EFLAGS_IF_1);
    proc_stack->esp = (uint32_t)get_a_page(PF_USER, USER_STACK3_VADDR) + PG_SIZE ;
    proc_stack->ss = 0x33;

    asm volatile ("movl %0, %%esp; jmp interrupt_exit" : : "g" (proc_stack) : "memory");


}



/* 击活页表 */
void page_dir_activate(struct task_struct* p_thread) {
/********************************************************
 * 执行此函数时,当前任务可能是线程。
 * 之所以对线程也要重新安装页表, 原因是上一次被调度的可能是进程,
 * 否则不恢复页表的话,线程就会使用进程的页表了。
 ********************************************************/
/* 若为内核线程,需要重新填充页表为2*1024*1024 */
   uint32_t pagedir_phy_addr = 2*1024*1024;  // 默认为内核的页目录物理地址,也就是内核线程所用的页目录表


   if (p_thread->pgdir != NULL)	{    // 用户态进程有自己的页目录表
      pagedir_phy_addr = addr_v2p((uint32_t)p_thread->pgdir);

   }

   // console_str("page_dir_activate");console_int_oct(pagedir_phy_addr);
   console_str("p");

   /* 更新页目录寄存器cr3,使新页表生效 */
   asm volatile ("movl %0, %%cr3" : : "r" (pagedir_phy_addr) : "memory");
}



/* 击活线程或进程的页表,更新tss中的esp0为进程的特权级0的栈 */
void process_activate(struct task_struct* p_thread) {
   ASSERT(p_thread != NULL);
   /* 击活该进程或线程的页表 */
   page_dir_activate(p_thread);
   // console_str("process_activate_p_thread ");console_int_oct((uint32_t)p_thread);
   // console_str("process_activate_p_thread_pgdir ");console_int_oct((uint32_t)p_thread->pgdir);

   /* 内核线程特权级本身就是0,处理器进入中断时并不会从tss中获取0特权级栈地址,故不需要更新esp0 */
   if (p_thread->pgdir) {
      /* 更新该进程的esp0,用于此进程被中断时保留上下文 */
      update_tss_esp(p_thread);
   }
}



void create_user_virtual_bitmap(struct task_struct* user_prog){
    user_prog->userprog_vaddr.virtual_addr_start = 0x8048000;

     // 定义虚拟 地址空间位图占用的内存页，向上取整
    uint32_t bitmap_pg_cnt = DIV_ROUND_UP((0xc0000000 - 0x8048000) / PG_SIZE / 8 , PG_SIZE);
    user_prog->userprog_vaddr.virtual_addr_bitmap.start_addr = get_kernel_pages(bitmap_pg_cnt);
    user_prog->userprog_vaddr.virtual_addr_bitmap.bytes_length = (0xc0000000 - 0x8048000) / PG_SIZE / 8;
    bitmap_init(&user_prog->userprog_vaddr.virtual_addr_bitmap);
}




uint32_t* create_page_dir(){
    uint32_t* page_dir_vaddr = get_kernel_pages(1);
   if (page_dir_vaddr == NULL) {
      console_str("create_page_dir: get_kernel_page failed!");
      return NULL;
   }

    /*  page_dir_vaddr + 0x300*4 是内核页目录的第768项 */
     memcpy((uint32_t*)((uint32_t)page_dir_vaddr + 0x300*4), (uint32_t*)(0xfffff000+0x300*4), 1024);
     uint32_t new_page_dir_phy_addr = addr_v2p((uint32_t)page_dir_vaddr);
     page_dir_vaddr[1023] = new_page_dir_phy_addr | PG_US_U | PG_RW_W | PG_P_1;


     // 由于代码是在低1M地址内，（代码编译地址就是从0开始的，page_dir_activate 切换cr3后，编译地址在1M以下）
     // 所以要复制低4M的页目录表
     memcpy(page_dir_vaddr, (uint32_t*)(0xfffff000), 4);


     return page_dir_vaddr;
}










void process_execute(void* filename, char* name){
    struct task_struct* p_thread = get_kernel_pages(1);
    init_thread(p_thread, name, 16);

    create_user_virtual_bitmap(p_thread);
    thread_create(p_thread, start_process, filename);
    p_thread->pgdir = create_page_dir();
    block_desc_init(p_thread->u_block_desc);


    enum interrupt_status old_status = interrupt_disable();
    ASSERT(!elem_find(&thread_ready_list, &p_thread->general_tag));
    list_append(&thread_ready_list, &p_thread->general_tag);

    ASSERT(!elem_find(&thread_all_list, &p_thread->all_list_tag));
    list_append(&thread_all_list, &p_thread->all_list_tag);
    set_interrupt_status(old_status);

    // console_str("process_execute  ");console_int_oct((uint32_t)p_thread);
    // console_str("addr_v2p  ");console_int_oct(addr_v2p((uint32_t)p_thread));
    //console_str("\n");
}

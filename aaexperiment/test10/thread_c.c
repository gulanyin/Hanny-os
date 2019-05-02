#include "thread_h.h"
#include "string_h.h"
#include "interrupt_h.h"
#include "debug_h.h"
#include "print_h.h"

extern unsigned int threa_0_task;
extern unsigned int threa_1_task;
extern unsigned int threa_2_task;
extern int current_thread_number;
void switch_to(struct task_struct*, struct task_struct*);

#define PG_SIZE 4096


/* 由kernel_thread去执行function(func_arg) */
static void kernel_thread(thread_func function, void* func_arg) {
/* 执行function前要开中断,避免后面的时钟中断被屏蔽,而无法调度其它线程 */
   interrupt_on();
   function(func_arg);
}


void create_thread(struct task_struct* pthread, thread_func function, void* func_arg){
      pthread->self_kstack -= sizeof(struct intr_stack);
      pthread->self_kstack -= sizeof(struct thread_stack);
      struct thread_stack* kthread_stack = (struct thread_stack*)pthread->self_kstack;
      kthread_stack->eip = kernel_thread;
      kthread_stack->function = function;
      kthread_stack->func_arg = func_arg;
      kthread_stack->ebp = kthread_stack->ebx = kthread_stack->esi = kthread_stack->edi = 0;

}


void init_thread(struct task_struct* pthread, char* name, int prio){
    strcpy(pthread->name, name);
    pthread->status = TASK_READY;
    pthread->self_kstack = (uint32_t*)((uint32_t)pthread + PG_SIZE);
    pthread->priority = prio;
    pthread->ticks = prio;
    pthread->elapsed_ticks = 0;
    pthread->pgdir = NULL;
    pthread->stack_magic = 0x19870916;	  // 自定义的魔数
}


struct task_struct* thread_start(int thread_number, char* name, int prio, thread_func function, void* func_arg) {
   struct task_struct* thread = NULL;

   if(thread_number == 1){
       thread = (struct task_struct*)&threa_1_task;

       // print_str("\\threa_1_task: ");
       // print_int_oct((unsigned int)&threa_1_task);

   }else if(thread_number == 2){
       thread = (struct task_struct*)&threa_2_task;
       // print_str("\\threa_2_task: ");
       // print_int_oct((unsigned int)&threa_2_task);


   }else if(thread_number == 0){
       thread = (struct task_struct*)&threa_0_task;

       // print_str("\threa_0_task: ");
       // print_int_oct((unsigned int)&threa_0_task);
       // print_str("\thread: ");
       // print_int_oct((unsigned int)thread);
   }

   // print_str("\thread: ");
   // print_int_oct((unsigned int)thread);
   ASSERT(thread != NULL);


   init_thread(thread, name, prio);
   create_thread(thread, function, func_arg);


   print_str("\nthread_number: ");
   print_int_oct((unsigned int)thread_number);
   print_str(" thread_addr: ");
   print_int_oct((unsigned int)thread);
   print_str(" name: ");
   print_str(thread->name);


   // print_str("\nthread: ");
   // print_int_oct((unsigned int)thread);
   print_char('\n');
   return thread;
}



/* 线程切换：
    初始化了3个线程，各自的线程结构体在threa_0_task， threa_1_task， threa_2_task 处
    0 号线程是从main线程切过来的，换了栈，抛弃原来的loaders.asm的内核栈，也不会切换到0号线程，因为数据结构堆栈有问题
    1 号线程切换到2号线程
    2号线程切换到1号线程


 */
void schedule() {
    interrupt_off();

    struct task_struct* cur = NULL;
    struct task_struct* next = NULL;
    print_str("\ncurrent_thread_number: ");
    print_int_oct((unsigned int)current_thread_number);
    print_char('\n');

   if(current_thread_number == 0){
       // 0 直接换到1
       cur = (struct task_struct*)&threa_0_task;
       next = (struct task_struct*)&threa_1_task;

       current_thread_number = 1;
   }else if(current_thread_number == 1){
       cur = (struct task_struct*)&threa_1_task;
       next = (struct task_struct*)&threa_2_task;
       current_thread_number =2;
   }else if(current_thread_number == 2){
       cur = (struct task_struct*)&threa_2_task;
       next = (struct task_struct*)&threa_1_task;
       current_thread_number = 1;
   }
   ASSERT(cur != NULL || next != NULL);
   print_str("next: ");
   print_int_oct((unsigned int)next);
   print_char('\n');
   print_str("name: ");

   print_str(next->name);
   print_char('\n');
   switch_to(cur, next);
}

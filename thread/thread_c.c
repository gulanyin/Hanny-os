#include "thread/thread_h.h"
#include "mm/init_memory_h.h"
#include "kernel/print_h.h"
#include "kernel/debug_h.h"
#include "kernel/list_h.h"
#include "kernel/interrupt_h.h"
#include "kernel/string_h.h"
#include "userprog/process_h.h"
#include "kernel/sync_h.h"




#define PG_SIZE 4096


extern void switch_to(struct task_struct* cur, struct task_struct* next);


struct task_struct* main_thread;    // 主线程PCB
struct task_struct* idle_thread;    // 主线程PCB
LIST thread_ready_list;	            // 就绪队列
LIST thread_all_list;	            // 所有任务队列
static PLIST_ELEM thread_tag;       // 用于保存队列中的线程结点


// 自动分配线程id
LOCK pid_lock;


// 分配pit
static uint32_t allocate_pid (){
    static uint32_t next_pid = 0;
    lock_acquire(&pid_lock);
    next_pid++;
    lock_release(&pid_lock);
    return next_pid;
}





// 当前线程的pcb，pcb总是在一个页内，根据栈来取
struct task_struct* running_thread() {
   uint32_t esp;
   asm ("mov %%esp, %0" : "=g" (esp));
   // 取esp整数部分即pcb起始地址
   return (struct task_struct*)(esp & 0xfffff000);
}



// 由kernel_thread去执行function(func_arg)
static void kernel_thread(thread_func function, void* func_arg) {
    // 执行function前要开中断,避免后面的时钟中断被屏蔽,而无法调度其它线程
    on_interrupt();
    function(func_arg);
}




/* 系统空闲时运行的线程 */
static void idle(void* arg __attribute__ ((unused))) {
   while(1) {
      thread_block(TASK_BLOCKED);
      //执行hlt时必须要保证目前处在开中断的情况下
      asm volatile ("sti; hlt" : : : "memory");
   }
}




// 初始化线程基本信息
void init_thread(struct task_struct* pthread, char* name, int prio) {
   memset(pthread, 0, sizeof(*pthread));
   strcpy(pthread->name, name);

   if (pthread == main_thread) {
       // 由于把main函数也封装成一个线程,并且它一直是运行的,故将其直接设为TASK_RUNNING
       pthread->status = TASK_RUNNING;
   } else {
      pthread->status = TASK_READY;
   }

   // self_kstack是线程自己在内核态下使用的栈顶地址
   pthread->self_kstack = (uint32_t*)((uint32_t)pthread + PG_SIZE);
   pthread->priority = prio;
   pthread->ticks = prio;
   pthread->elapsed_ticks = 0;
   pthread->pgdir = NULL;
   pthread->cwd_inode_nr = 0;
   pthread->stack_magic = 0x19870916;	  // 自定义的魔数

   pthread->pid = allocate_pid();

   pthread->fd_table[0] = 0;
   pthread->fd_table[1] = 1;
   pthread->fd_table[2] = 2;

   uint8_t fd_index = 3;
   while(fd_index < 8){
       pthread->fd_table[fd_index] = -1;
       fd_index++;
   }
}


// 初始化线程栈thread_stack,将待执行的函数和参数放到thread_stack中相应的位置
void thread_create(struct task_struct* pthread, thread_func function, void* func_arg) {
   // 先预留中断使用栈的空间,可见thread.h中定义的结构
   pthread->self_kstack -= sizeof(struct intr_stack);

   /* 再留出线程栈空间,可见thread.h中定义 */
   pthread->self_kstack -= sizeof(struct thread_stack);
   struct thread_stack* kthread_stack = (struct thread_stack*)pthread->self_kstack;
   kthread_stack->eip = kernel_thread;
   kthread_stack->function = function;
   kthread_stack->func_arg = func_arg;
   kthread_stack->ebp = kthread_stack->ebx = kthread_stack->esi = kthread_stack->edi = 0;
}



// 创建一个优先级为prio的线程,线程名为name,线程所执行的函数是function(func_arg)
struct task_struct* thread_start(char* name, int prio, thread_func function, void* func_arg) {
    // pcb都位于内核空间,包括用户进程的pcb也是在内核空间
   struct task_struct* thread = get_kernel_pages(1);

   //print_str("========start thread ");print_int_oct((uint32_t)thread);
   init_thread(thread, name, prio);
   thread_create(thread, function, func_arg);

   // 确保之前不在队列中
   ASSERT(!elem_find(&thread_ready_list, &thread->general_tag));
   // 加入就绪线程队列
   list_append(&thread_ready_list, &thread->general_tag);

   // 确保之前不在队列中
   ASSERT(!elem_find(&thread_all_list, &thread->all_list_tag));
   // 加入全部线程队列
   list_append(&thread_all_list, &thread->all_list_tag);

   return thread;
}




// 将kernel中的main函数完善为主线程
static void make_main_thread(void) {
    // 在boot/headers.asm 中，初始化了一块内存栈，这个内存栈设为2M前倒数第二个页面， 即栈底为0x1ff000
   main_thread = running_thread();

   print_str("========main_thread ");print_int_oct((uint32_t)main_thread);

   init_thread(main_thread, "main", 31);

 //main函数是当前线程,当前线程不在thread_ready_list中,
 // 所以只将其加在thread_all_list中.
   ASSERT(!elem_find(&thread_all_list, &main_thread->all_list_tag));
   list_append(&thread_all_list, &main_thread->all_list_tag);
}



// 调度
void schedule(){
    // 关中断
    off_interrupt();

    // 当前线程
    struct task_struct* cur = running_thread();
    //print_str("\n========cur---cur ");print_int_oct((uint32_t)cur);
    if (cur->status == TASK_RUNNING) {
        // 若此线程只是cpu时间片到了,将其加入到就绪队列尾
        ASSERT(!elem_find(&thread_ready_list, &cur->general_tag));
        list_append(&thread_ready_list, &cur->general_tag);
        cur->ticks = cur->priority;     // 重新将当前线程的ticks再重置为其priority;
        cur->status = TASK_READY;
   } else {
      /* 若此线程需要某事件发生后才能继续上cpu运行,
      不需要将其加入队列,因为当前线程不在就绪队列中。*/
   }

   // ASSERT(!list_empty(&thread_ready_list));
   if (list_empty(&thread_ready_list)) {
         thread_unblock(idle_thread);
   }


   thread_tag = NULL;	  // thread_tag清空
   // 将thread_ready_list队列中的第一个就绪线程弹出,准备将其调度上cpu.
   thread_tag = list_pop(&thread_ready_list);
   // struct task_struct* next = (struct task_struct*) (   0xfffff000 & ((uint32_t)&thread_tag)  );
   struct task_struct* next = (struct task_struct*) (   0xfffff000 & ((uint32_t)thread_tag)  );
  // print_str("  ========next ");print_int_oct((uint32_t)next);
   // print_char('s');
   next->status = TASK_RUNNING;

   process_activate(next);

   switch_to(cur, next);

}



// 线程阻塞自己
void thread_block(enum task_status status){
    ASSERT(((status == TASK_BLOCKED) || (status == TASK_WAITING) || (status == TASK_HANGING)));
    enum interrupt_status old_status = interrupt_disable();
    struct task_struct* cur_thread = running_thread();
    cur_thread->status = status; // 置其状态为stat
    schedule();		      // 将当前线程换下处理器
    //待当前线程被解除阻塞后才继续运行下面的
    set_interrupt_status(old_status);
}


// 将线程pthread解除阻塞
void thread_unblock(struct task_struct* pthread) {
   enum interrupt_status old_status = interrupt_disable();
   ASSERT(((pthread->status == TASK_BLOCKED) || (pthread->status == TASK_WAITING) || (pthread->status == TASK_HANGING)));
   if (pthread->status != TASK_READY) {
        ASSERT(!elem_find(&thread_ready_list, &pthread->general_tag));
        if (elem_find(&thread_ready_list, &pthread->general_tag)) {
            PANIC("thread_unblock: blocked thread in ready_list\n");
        }
      list_push(&thread_ready_list, &pthread->general_tag);    // 放到队列的最前面,使其尽快得到调度
      pthread->status = TASK_READY;
   }
   set_interrupt_status(old_status);
}


/* 主动让出cpu,换其它线程运行 */
void thread_yield(void) {
   struct task_struct* cur = running_thread();
   enum interrupt_status old_status = interrupt_disable();
   ASSERT(!elem_find(&thread_ready_list, &cur->general_tag));
   list_append(&thread_ready_list, &cur->general_tag);
   cur->status = TASK_READY;
   schedule();
   set_interrupt_status(old_status);
}



void entry_init_thread(){
    print_str("thread_init start\n");
    init_list(&thread_ready_list);
    init_list(&thread_all_list);
    lock_init(&pid_lock);
    print_str("==&thread_ready_list ");print_int_oct((uint32_t)&thread_ready_list);
    print_str("==&thread_all_list ");print_int_oct((uint32_t)&thread_all_list);
    // 将当前main函数创建为线程
    make_main_thread();
    /* 创建idle线程 */
    idle_thread = thread_start("idle", 10, idle, NULL);
    print_str("thread_init done\n");

}

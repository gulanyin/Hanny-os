#include "sys/std_int_h.h"
#include "sys/gloab_h.h"

#include "kernel/sync_h.h"
#include "kernel/list_h.h"
#include "kernel/interrupt_h.h"
#include "kernel/debug_h.h"




// 初始化信号量
void semaphore_init(PSEMAPHORE p_semaphore, uint8_t value){
    p_semaphore->value = value;
    init_list(&p_semaphore->waiters);
}

// 初始化锁
void lock_init(PLOCK p_lock){
    p_lock->holder = NULL;
    p_lock->holder_repeat_nr = 0;
    semaphore_init(&p_lock->semaphore, 1);
}



// 信号量 down
void semaphore_down(PSEMAPHORE p_semaphore){
     enum interrupt_status old_status = interrupt_disable();
     while(p_semaphore->value == 0){
        ASSERT(!elem_find(&p_semaphore->waiters, &running_thread()->general_tag) );
        // 当前线程不应该已在信号量的waiters队列中
        if (elem_find(&p_semaphore->waiters, &running_thread()->general_tag)) {
            PANIC("sema_down: thread blocked has been in waiters_list\n");
        }
        // 若信号量的值等于0,则当前线程把自己加入该锁的等待队列,然后阻塞自己
        list_append(&p_semaphore->waiters, &running_thread()->general_tag);
        thread_block(TASK_BLOCKED);    // 阻塞线程,直到被唤醒
        // 线程唤醒后从此处开始执行， thread_block switch_to 切换到其他线程
     }

     p_semaphore->value--;
     ASSERT(p_semaphore->value == 0);

     set_interrupt_status(old_status);
}


// 信号量up
void semaphore_up(PSEMAPHORE p_semaphore){
    enum interrupt_status old_status = interrupt_disable();
    ASSERT(p_semaphore->value == 0);
    if (!list_empty(&p_semaphore->waiters)) {
        struct task_struct* thread_blocked = (struct task_struct*) (   0xfffff000 & ((uint32_t)list_pop(&p_semaphore->waiters))  );
        thread_unblock(thread_blocked);
    }
    p_semaphore->value++;
    ASSERT(p_semaphore->value == 1);

    set_interrupt_status(old_status);
}


// 获取锁
void lock_acquire(PLOCK p_lock){
    if (p_lock->holder != running_thread()) {
        semaphore_down(&p_lock->semaphore);    // 对信号量P操作,原子操作
        p_lock->holder = running_thread();
        ASSERT(p_lock->holder_repeat_nr == 0);
        p_lock->holder_repeat_nr = 1;
    } else {
        p_lock->holder_repeat_nr++;
    }
}



// 释放锁
void lock_release(PLOCK p_lock) {
   ASSERT(p_lock->holder == running_thread());
   if (p_lock->holder_repeat_nr > 1) {
      p_lock->holder_repeat_nr--;
      return;
   }
   ASSERT(p_lock->holder_repeat_nr == 1);

   p_lock->holder = NULL;	   // 把锁的持有者置空放在V操作之前
   p_lock->holder_repeat_nr = 0;
   semaphore_up(&p_lock->semaphore);	   // 信号量的V操作,也是原子操作
}

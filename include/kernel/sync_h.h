#ifndef __KERNEL_SYNC_H
#define __KERNEL_SYNC_H

#include"sys/gloab_h.h"
#include"sys/std_int_h.h"
#include"kernel/list_h.h"
#include"thread/thread_h.h"


typedef struct _SEMAPHORE {
    uint8_t value;
    LIST waiters;
}SEMAPHORE, *PSEMAPHORE;

typedef struct _LOCK {
    struct task_struct* holder;
    SEMAPHORE semaphore;
    uint32_t holder_repeat_nr;
}LOCK, *PLOCK;



void semaphore_init(PSEMAPHORE p_semaphore, uint8_t vlaue);
void lock_init(PLOCK p_lock);
void semaphore_down(PSEMAPHORE p_semaphore);
void semaphore_up(PSEMAPHORE p_semaphore);
void lock_acquire(PLOCK p_lock);
void lock_release(PLOCK p_lock);



#endif

#ifndef __USERPROG_PROCESS_H
#define __USERPROG_PROCESS_H

#include  "thread/thread_h.h"

void process_execute(void* filename, char* name);
void process_activate(struct task_struct* p_thread);

#endif

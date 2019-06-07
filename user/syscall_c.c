#include "sys/std_int_h.h"
#include "device/console_h.h"
#include "thread/thread_h.h"

// 调用子功能号e
#define SYSCALL_GETPID 0


#define SYSCALL_TABLE_NUMBER  0x01
typedef void (*syscall_handler)(void* arg1, void* arg2, void* arg3);
// 系统调用子功能号对应的处理函数
syscall_handler interrupt_handler_syscall_table[SYSCALL_TABLE_NUMBER];
// 注册系统调用子功能号
void register_syscall_handler(int syscall_number, void* _haneler){
    syscall_handler haneler = (syscall_handler)_haneler;
    interrupt_handler_syscall_table[syscall_number] = haneler;
}





/* 无参数的系统调用 */
#define _syscall0(NUMBER) ({				       \
   int retval;					               \
   asm volatile (					       \
   "int $0x22"						       \
   : "=a" (retval)					       \
   : "a" (NUMBER)					       \
   : "memory"						       \
   );							       \
   retval;						       \
})



/* 返回当前任务pid */
uint32_t getpid() {
   return _syscall0(SYSCALL_GETPID);
}


uint32_t syscall_getpid(){
    console_str("syscall_getpid ok, pid is: ");
    struct task_struct* cur = running_thread();
    console_int_oct(cur->pid);
    console_str("\n");

    return 1;
}


void init_all_syscall(){
    register_syscall_handler(SYSCALL_GETPID, syscall_getpid);
}

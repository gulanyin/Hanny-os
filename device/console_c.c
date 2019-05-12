#include"device/console_h.h"
#include"kernel/print_h.h"
#include"kernel/sync_h.h"


static LOCK console_lock;


// 初始化console
void entyr_console_init(){
    lock_init(&console_lock);
}


// 获取console
void console_acquire(){
    lock_acquire(&console_lock);
}

// 释放console
void console_release(){
    lock_release(&console_lock);
}

// console 打印字符串
void console_str(char* str){
    console_acquire();
    print_str(str);
    console_release();
}


// 打印单个字符
void console_char(char ch){
    console_acquire();
    print_char(ch);
    console_release();
}


// 打印整数
void console_int_oct(int number){
    console_acquire();
    print_int_oct(number);
    console_release();
}

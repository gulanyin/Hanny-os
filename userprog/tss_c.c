#include "userprog/tss_h.h"
#include "thread/thread_h.h"
#include "sys/std_int_h.h"
#include "kernel/print_h.h"



TSS tss;

extern WORD protect_model_gdt;


void update_tss_esp(struct task_struct * p_thread){
    tss.esp0 = (uint32_t*)( (uint32_t)p_thread + 4096 );
}


void entry_init_tss(){
    //print_str("tss ");print_int_oct((uint32_t)&tss);

    WORD*  p_addres = (WORD*)&protect_model_gdt;
    //print_str("p_addres ");print_int_oct((uint32_t)p_addres);

    //print_char('\n');print_int_oct(*(p_addres+4*4 + 0));
    //print_char('\n');print_int_oct(*(p_addres+4*4 + 1));

    tss.ss0 = 0x10;
    tss.io_base = sizeof(tss);

    *(p_addres+4*4 + 1) = (WORD)&tss;
    //print_char('\n');print_int_oct(*(p_addres+4*4 + 1));
    //print_char('\n');print_int_oct(*(p_addres+4*4 + 2));
    //print_char('\n');print_int_oct(*(p_addres+4*4 + 3));

    asm volatile ("ltr %w0" : : "r" (0x20));
    print_str("entry_init_tss ok ");


    // 停在此处可已调试查看是否设置成功
    //while(1);

}

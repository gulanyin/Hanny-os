#include "userprog/tss_h.h"
#include "thread/thread_h.h"
#include "sys/std_int_h.h"
#include "kernel/print_h.h"



TSS tss;

extern WORD protect_model_gdt;


void update_tss_esp(struct task_struct * p_thread){
    tss.esp0 = (uint32_t*)( (uint32_t)p_thread + 4096 );
    // print_str("tss.esp0 ");print_int_oct((uint32_t)tss.esp0);
}


void entry_init_tss(){
    uint32_t tss_addr1 = (uint32_t)&tss;
    WORD tss_addr_0_15 = (WORD) (tss_addr1);
    WORD tss_addr_16_31 = (WORD) (tss_addr1 >> 16);
    WORD tss_addr_24_31 = tss_addr_16_31 & 0xff00;
    WORD tss_addr_16_23 = tss_addr_16_31 & 0x00ff;
    // print_str("tss ");print_int_oct((uint32_t)&tss);
    // print_str("tss ");print_int_oct((uint32_t)tss_addr1);
    // print_str("tss ");print_int_oct((uint32_t)tss_addr2);

    WORD*  p_addres = (WORD*)&protect_model_gdt;
    // print_str("protect_model_gdt ");print_int_oct((uint32_t)p_addres);

    // print_str("4* ");print_int_oct(*(p_addres+4*4 + 0));
    // print_str("4* ");print_int_oct(*(p_addres+4*4 + 1));

    tss.ss0 = 0x10;
    tss.io_base = sizeof(tss);

    // *(p_addres+4*4 + 1) = (WORD)&tss;
    *(p_addres+4*4 + 1) = tss_addr_0_15;
    *(p_addres+4*4 + 2) = *(p_addres+4*4 + 2) | tss_addr_16_23;
    *(p_addres+4*4 + 3) = *(p_addres+4*4 + 3) | tss_addr_24_31;
    // print_str(" 4*4+1 ");print_int_oct(*(p_addres+4*4 + 1));
    // print_str(" 4*4+2 ");print_int_oct(*(p_addres+4*4 + 2));
    // print_str(" 4*4+3 ");print_int_oct(*(p_addres+4*4 + 3));

    asm volatile ("ltr %w0" : : "r" (0x20));
    print_str("entry_init_tss ok ");


    // 停在此处可已调试查看是否设置成功
    //while(1);

}

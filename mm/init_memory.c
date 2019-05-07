#include "sys/mm_h.h"

#include "kernel/print_h.h"
#include "kernel/debug_h.h"
#include "kernel/string_h.h"

#define PAGE_SIZE 4096
#define PG_ATTRIBUTE_S_R_W_P 3   // u/s = 0, r/w=1, p=1



// 初始化分页
static void init_page(){
    int i=0 , page_start=0;
    int mem_number = *((int*)(0x85000));
    PMEMORY_DESCRIPTOR p_mm = (PMEMORY_DESCRIPTOR)0x85010;
    print_int_oct(       mem_number     );
    print_char('\n');

    for(i=0; i<mem_number; i++){
        // 找出可用内存能访问的最大地址
        print_str("addr ");
        print_int_oct(       (p_mm + i)->add_low     );
        print_str("   len ");
        print_int_oct(       (p_mm + i)->len_hig     );
        print_int_oct(       (p_mm + i)->len_low     );
        print_str("   type ");
        print_int_oct(       (p_mm + i)->type     );
        print_char('\n');
    }

    memset((void *) 0x100000, 0, 4096*2);  //页目录表和页表清空
    // pde 在1M内存起始的第一页物理内存里面,第一个页表在页目录后
    unsigned int * p_pde_start = (unsigned int *) 0x100000;
    unsigned int * p_first_pte_start = (unsigned int *) 0x101000;


    // 第一个页目录项指向第一个页表,
    *(p_pde_start + 0) = (0x101000  | PG_ATTRIBUTE_S_R_W_P);
    *(p_pde_start + 768) =  (0x101000  | PG_ATTRIBUTE_S_R_W_P);
    // 第一个页表项指向物理内存0处， 一个页表能指向1k* 4k = 4M内存, 内核代码从内存0起始大约几十kb
    // 前256个页表项指向 0-1M的物理内存
    *p_first_pte_start = 0x0;
    for(i=0; i<256; i++){
        *(p_first_pte_start + i) = (  (PAGE_SIZE * i)  ) | PG_ATTRIBUTE_S_R_W_P;
    }

    // 开启分页
    // 设置cr3
    asm volatile ( "push %eax");
    asm volatile ( "mov $0x00100000, %eax");
    asm volatile ( "mov %eax, %cr3");

    //即将CR0寄存器的第31位置1
    asm volatile ( "mov %cr0, %eax");
    asm volatile ( "or $0x80000000, %eax");
    asm volatile ( "mov %eax, %cr0");
    asm volatile ( "pop %eax");
    print_str("\n page ok ");

    ASSERT(1==2);

}





void init_memory(){
    // 根据存放在0x85000处的内存信息初始化内存管理
    int i = 0;
    int max_access_addr = 0; // 最大能访问到的内存, 最大不超过4GB

    print_char('\n');
    print_str("init memory start");

    int mem_number = *((int*)(0x85000));
    PMEMORY_DESCRIPTOR p_mm = (PMEMORY_DESCRIPTOR)0x85010;




    // 获取内存信息失败
    if(mem_number == 0){
        print_str("mem_number is 0, init memory failed");
        while(1);
    }

    // 获取内存信息成功，则判断是否小于32M
    for(i=0; i<mem_number; i++){
        // 找出可用内存能访问的最大地址
        if( ((p_mm + i)->type  == 1) && max_access_addr<( (p_mm + i)->add_low + (p_mm + i)->len_low) ){
            max_access_addr = (p_mm + i)->add_low + (p_mm + i)->len_low;
        }
    }

    if(max_access_addr < 32*1024*1024){
        print_str("less than 32M, init memory failed");
        while(1);
    }


    // 分页设置
    init_page();



}

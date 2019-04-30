#ifndef __KERNEL_GLOBAL_H
#define __KERNEL_GLOBAL_H


#define RPL0 0
#define RPL1 1
#define RPL2 2
#define RPL3 3

#define TI_GDT 0
#define TI_LDT 1


#define SELECTOR_0_CODE  ((1 << 3) + (TI_GDT << 2) + RPL0)  // 0000 0000 0000 1000
#define SELECTOR_0_DATA  ((2 << 3) + (TI_GDT << 2) + RPL0)  // 0000 0000 0001 0000
#define SELECTOR_0_STACK   SELECTOR_0_DATA
#define SELECTOR_0_GS	   ((3 << 3) + (TI_GDT << 2) + RPL0)// 0000 0000 0001 1000


// IDT 描述符属性
#define IDT_DESC_P 1
#define IDT_DESC_DPL0 0
#define IDT_DESC_DPL3 3
#define	 IDT_DESC_32_TYPE     0xE   // 32位的门
#define	 IDT_DESC_16_TYPE     0x6   // 16位的门
#define	 IDT_DESC_ATTR_DPL0  ((IDT_DESC_P << 7) + (IDT_DESC_DPL0 << 5) + IDT_DESC_32_TYPE)  // 1000 1110
#define	 IDT_DESC_ATTR_DPL3  ((IDT_DESC_P << 7) + (IDT_DESC_DPL3 << 5) + IDT_DESC_32_TYPE)  // 1110 1110


typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int int64_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;
typedef int intrrupt_handler;
#endif

#define IDT_DESC_COUNT 0x21
void idt_init(void);



// 门描述符
struct gate_desc{
    uint16_t handler_offset_low_word;
    uint16_t selector;
    uint8_t param_count;
    uint8_t attribute;
    uint16_t handler_offset_high_word;
};



void print_char(unsigned char);
void print_str(unsigned char*);
void print_int_oct(unsigned int);
void set_idt_lidt(int, int);


//
static void set_idt_desc(struct gate_desc* p_gate_desc, uint8_t attr, intrrupt_handler function);
static struct gate_desc idt[IDT_DESC_COUNT];
extern intrrupt_handler interrupt_des_table[IDT_DESC_COUNT];


static void set_idt_desc(struct gate_desc* p_gate_desc, uint8_t attr, intrrupt_handler function){
    p_gate_desc->handler_offset_low_word = (uint32_t)function & 0x0000FFFF;
    p_gate_desc->selector = SELECTOR_0_CODE;
    p_gate_desc->param_count = 0;
    p_gate_desc->attribute = attr;
    p_gate_desc->handler_offset_high_word = ((uint32_t)function & 0xFFFF0000) >> 16;

}


static void idt_desc_init(){
    int i;
    for(i=0; i<=IDT_DESC_COUNT; i++){
        set_idt_desc(&idt[i], IDT_DESC_ATTR_DPL0, interrupt_des_table[i]);
    }
    print_str("\n\nset_idt_desc done\n");
}


static void pic_init(){
    // 设置8259A  在汇编中实现
}


void idt_init(){
    print_str("\nstart idt init");
    idt_desc_init();
    pic_init();
    //
    // int size_of_idt = (sizeof(idt) - 1);
    int size_of_idt = (0x20*8 - 1);
    uint64_t idt_operand = ( (uint64_t)(sizeof(idt) - 1)  | (uint64_t)(  (uint32_t)idt << 16)  );
    //
    // // 设置idtr寄存器
    set_idt_lidt((int)&idt, size_of_idt);
    // asm volatile("lidt %0": : "m"(idt_operand));
    // asm volatile("lidt %0" : : "m" (idt_operand));
    // __asm__ __volatile__("lidt %0": "=m" (idt_operand));
    // asm volatile("lidt idt_operand");
    print_str("\nstart idt done");
    asm volatile ("sti");
    //

}

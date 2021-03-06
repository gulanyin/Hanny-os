
#include "kernel/print_h.h"
#include "sys/std_int_h.h"
#include "kernel/interrupt_h.h"
#include "kernel/io.h"
#include "thread/thread_h.h"
#include "kernel/debug_h.h"
#include "kernel/command_h.h"

#define EFLAGS_IF   0x00000200       // eflags寄存器中的if位为1
#define GET_EFLAGS(EFLAG_VAR) asm volatile("pushfl; popl %0" : "=g" (EFLAG_VAR))
#define VECTOR_NUMBER  0x30
typedef void (*p_interrupt_handler)(int vector_number);
p_interrupt_handler interrupt_handler_table[VECTOR_NUMBER];

static int number_time = 0;   // 时钟滴答数
uint32_t kernel_ticks = 0;


/**   键盘中断   **/
#define esc_char 0x1b   // esc ascii 0x1b
#define backspace_char '\b'
#define tab_char '\t'
#define enter_char '\r'
#define delete_char 0x7f  // esc ascii 0x1b


// 不可见字符
#define invisible_char 0
#define ctrl_l_char invisible_char   // left ctrl
#define ctrl_r_char invisible_char

#define shift_l_char invisible_char
#define shift_r_char invisible_char

#define alt_l_char invisible_char
#define alt_r_char invisible_char
#define caps_lock_char invisible_char

// 控制键通码和断码
#define ctrl_l_make 0x1d
#define ctrl_l_break 0x9d
#define ctrl_r_make 0xe01d
#define ctrl_r_break 0xe09d

#define shift_l_make 0x2a
#define shift_l_break 0xaa
#define shift_r_make 0x36
#define shift_r_break 0xb6


#define alt_l_make 0x38
#define alt_l_break 0xb8
#define alt_r_make 0xe038
#define alt_r_break 0xe0b8


#define backspace_make 0x0e
#define backspace_break 0x8e

#define enter_make 0x1c
#define enter_break 0x9c


#define scaps_lock_make 0x3a
#define scaps_lock_break 0xba


#define KBD_BUF_PORT 0x60	   // 键盘buffer寄存器端口号为0x60

// 全局变量，记录ctrl, shift, alt, caps_lock 状态, ext_scancode 记录是否以0xe0开头
static uint8_t ctrl_status, shift_status, alt_status, caps_lock_status, ext_scancode;
static char keymap[][2] = {
        //0x00 ---- 0x07
        {0, 0},
        {esc_char, esc_char},
        {'1', '!'},
        {'2', '@'},
        {'3', '#'},
        {'4', '$'},
        {'5', '%'},
        {'6', '^'},

        //0x08 ---- 0x0f
        {'7', '&'},
        {'8', '*'},
        {'9', '('},
        {'0', ')'},
        {'-', '_'},
        {'=', '+'},
        {backspace_char, backspace_char},
        {tab_char, tab_char},

        //0x10 ---- 0x17
        {'q', 'Q'},
        {'w', 'W'},
        {'e', 'E'},
        {'r', 'R'},
        {'t', 'T'},
        {'y', 'Y'},
        {'u', 'U'},
        {'i', 'I'},

        //0x18 ---- 0x1f
        {'o', 'O'},
        {'p', 'P'},
        {'[', '{'},
        {']', '}'},
        {enter_char, enter_char},
        {ctrl_l_char, ctrl_l_char},
        {'a', 'A'},
        {'s', 'S'},

        //0x20 ---- 0x27
        {'d', 'D'},
        {'f', 'F'},
        {'g', 'G'},
        {'h', 'H'},
        {'j', 'J'},
        {'k', 'K'},
        {'l', 'L'},
        {';', ':'},

        //0x28 ---- 0x2f
        {'\'', '"'},
        {'`', '~'},
        {shift_l_char, shift_l_char},
        {'\\', '|'},
        {'z', 'Z'},
        {'x', 'X'},
        {'c', 'C'},
        {'v', 'V'},

        //0x30 ---- 0x37
        {'b', 'B'},
        {'n', 'N'},
        {'m', 'M'},
        {',', '<'},
        {'.', '>'},
        {'/', '?'},
        {shift_r_char, shift_r_char},
        {'*', '*'},

        //0x38 ---- 0x3a
        {alt_l_char, alt_l_char},
        {' ', ' '},
        {caps_lock_char, caps_lock_char},
};



// 获取当前中断的状态
enum interrupt_status get_interrupt_status() {
   uint32_t eflags = 0;
   GET_EFLAGS(eflags);
   return (EFLAGS_IF & eflags) ? INTERRUPT_ON : INTERRUPT_OFF;
}

// 开中断并返回开中断前的状态
enum interrupt_status interrupt_enable() {
   enum interrupt_status old_status = get_interrupt_status();
   if (INTERRUPT_OFF == old_status) {
      asm volatile("sti");// 开中断,sti指令将IF位置1
   }
   return old_status;
}

// 关中断,并且返回关中断前的状态
enum interrupt_status interrupt_disable() {
   enum interrupt_status old_status = get_interrupt_status();
   if (INTERRUPT_ON == old_status) {
      asm volatile("cli" : : : "memory"); // 关中断,cli指令将IF位置0
   }
   return old_status;
}

// 设置中断状态
enum interrupt_status set_interrupt_status(enum interrupt_status status) {
   return status & INTERRUPT_ON ? interrupt_enable() : interrupt_disable();
}




// 默认处理
void hander_default(int vector_number){
    print_str("ERROR_default_hander  ");
    print_int_oct((unsigned int)vector_number);

    if (vector_number == 14) {	  // 若为Pagefault,将缺失的地址打印出来并悬停
        unsigned int page_fault_vaddr = 0;
        asm ("movl %%cr2, %0" : "=r" (page_fault_vaddr));	  // cr2是存放造成page_fault的地址
        print_str("\npage fault addr is ");print_int_oct(page_fault_vaddr);
    }
    print_str("\n!!!!!!!      excetion message end    !!!!!!!!\n");
    while(1);
}

/** 时钟中断 **/
void handler_time(int vector_number){
    // print_str("time_handle  ");
    struct task_struct* cur_thread = running_thread();

    ASSERT(cur_thread->stack_magic == (uint32_t)0x19870916);         // 检查栈是否溢出


    cur_thread->elapsed_ticks++;	  // 记录此线程占用的cpu时间嘀
    kernel_ticks++;	  //从内核第一次处理时间中断后开始至今的滴哒数,内核态和用户态总共的嘀哒数

    if (cur_thread->ticks == 0) {	  // 若进程时间片用完就开始调度新的进程上cpu
      schedule();
    } else {				  // 将当前进程的时间片-1
      cur_thread->ticks--;
    }


    // if(number_time < 100){
    //     number_time ++;
    // }else{
    //     number_time = 0;
    //     print_int_oct((unsigned int)vector_number);
    //     print_str("time_handler");
    // }
}

/** 键盘中断 **/
void handler_keyboard(int vector_number){

    uint8_t is_break_code;
    uint8_t make_code;
    uint16_t scancode = inb(KBD_BUF_PORT);

    uint16_t index_x, index_y = 0;
    unsigned char out_put_char;

    // print_int_oct((unsigned int)scancode);

    if(scancode == 0xe0){
        ext_scancode = 1;
        return ;
    }

    if(ext_scancode){
        scancode = ( 0xe000 | scancode);
        ext_scancode = 0;
    }

    is_break_code = ( (scancode & 0x0080) != 0);
    if(is_break_code){
        // 是断码
        if(scancode == ctrl_l_break || scancode == ctrl_r_break){
            ctrl_status = 0;
        }else if(scancode == shift_l_break || scancode == shift_r_break){
            shift_status = 0;
        }else if(scancode == alt_l_break || scancode == alt_r_break){
            alt_status = 0;
        }

        return;
    }else{
        // 不是断码

        // 控制码
        if (scancode == alt_l_make || scancode == alt_r_make){
            alt_status = 1;
            return ;
        }

        if (scancode == ctrl_l_make || scancode == ctrl_r_make){
            ctrl_status = 1;
            return ;
        }

        if (scancode == shift_l_make || scancode == shift_r_make){
            shift_status = 1;
            return ;
        }

        if (scancode == scaps_lock_make || scancode == scaps_lock_make){
            caps_lock_status = !caps_lock_status;
            return ;
        }

        // keymap
        if(scancode <= 0x3a && scancode >=0x00 ){
            index_x = scancode;
            index_y = (shift_status || caps_lock_status) ? 1 : 0;

            out_put_char = keymap[index_x][index_y];
            if(out_put_char){
                print_char(out_put_char);
                append_char_to_command(out_put_char);
            }

            return ;
        }

        print_str("unknown key\n");
    }
}



void register_handler(int vector_number, p_interrupt_handler haneler){
    interrupt_handler_table[vector_number] = haneler;
}


void register_extern_handler(int vector_number, void* _haneler){
    p_interrupt_handler  haneler = (p_interrupt_handler)_haneler;
    interrupt_handler_table[vector_number] = haneler;
}



void register_int_all(){
    int i = 0;
    for(i=0; i<VECTOR_NUMBER; i++){
        register_handler(i, hander_default);
    }

    // 时钟中断
    register_handler(0x20, handler_time);

    // 按键中断
    register_handler(0x21, handler_keyboard);
}




void entry_init_interrupt(){
    register_int_all();
    setup_interrupt();
    key_borard_command_init();

    // 开中断
    // on_interrupt();
}

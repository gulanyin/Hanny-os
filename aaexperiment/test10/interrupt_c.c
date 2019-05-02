
#include "print_h.h"
#include "std_int_h.h"
#include "interrupt_h.h"
#include "thread_h.h"

static int number_time = 0;   // 时钟滴答数
// 中断处理函数
void interrupt_exception_handler(int vector_number){

    int i = 0;

    unsigned char* err_message[] = {
        "ERROR_0",
    };
    for(i=0; i<400; i++){
        print_char('T');
    }
    print_int_oct((unsigned int)number_time);
    print_char('\n');

    if(vector_number == 0x20){
        if(number_time < 100){
            number_time ++;
        }else{
            number_time = 0;


            print_int_oct((unsigned int)vector_number);
            print_str(err_message[0]);

            // 切换线程
            schedule();
        }
    }else{
        print_int_oct((unsigned int)vector_number);
        print_str(err_message[0]);
    }
}

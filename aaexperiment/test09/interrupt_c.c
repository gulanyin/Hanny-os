
#include "print_h.h"

static int number_time = 0;

// 中断处理函数
void interrupt_exception_handler(int vector_number, int err_code){

    unsigned char* err_message[] = {
        "ERROR_0",
    };
    if(vector_number == 0x20){
        if(number_time < 100){
            number_time ++;
        }else{
            number_time = 0;
            print_int_oct((unsigned int)vector_number);
            print_str(err_message[0]);
        }
    }else{
        print_int_oct((unsigned int)vector_number);
        print_str(err_message[0]);
    }
}

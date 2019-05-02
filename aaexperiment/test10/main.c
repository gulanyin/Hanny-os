#include "debug_h.h"
#include "print_h.h"
#include "interrupt_h.h"
#include "thread_h.h"


extern unsigned int threa_0_task;
extern unsigned int threa_1_task;
extern unsigned int threa_2_task;
static int max = 0xFFFFFFFF;

void k_thread_0(void* args){
    int i = 0, j=0, k=0;
    while(1){
        for(i=0; i<max; i++){
            for(j=0; j<max;j++){
                for(k=0; k<max; k++){

                }
            }
        }
        print_str(args);
    }
}

void k_thread_1(void* args){
    int i = 0, j=0, k=0;
    while(1){
        for(i=0; i<max; i++){
            for(j=0; j<max;j++){
                for(k=0; k<max; k++){

                }
            }
        }
        print_str(args);
    }
    // schedule();
}

void k_thread_2(void* args){
    int i = 0, j=0, k=0;
    // schedule();
    while(1){
        for(i=0; i<max; i++){
            for(j=0; j<max;j++){
                for(k=0; k<max; k++){

                }
            }
        }
        print_str(args);
    }
}


void main(){
    print_char('M');
    // char dsdsdd[] = {'m', 'a', 'i', 'n', '\0'};
    print_str("main function\n");
    // print_str(dsdsdd);

    init_interrupt();

// print_int_oct((unsigned int)&threa_0_task);
// print_int_oct(threa_1_task);
// print_int_oct(threa_2_task);

    print_str("init interrupt ok!\n");




    thread_start(0, "k_thread_a", 31, k_thread_0, "argA ");
    thread_start(1, "k_thread_b", 8, k_thread_1, "BBBB ");
    thread_start(2, "k_thread_c", 8, k_thread_2, "CCCC ");

    print_str("init 3 threads ok!\n");


    // ASSERT(2==3);
}

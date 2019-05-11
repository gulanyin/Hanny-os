#ifndef __KERNEL_DEBUG_H
#define __KERNEL_DEBUG_H
static inline void on_interrupt(){
    asm volatile ( "sti");
}

static inline void off_interrupt(){
    asm volatile ( "cli");
}



void setup_interrupt();
void init_interrupt();

#endif

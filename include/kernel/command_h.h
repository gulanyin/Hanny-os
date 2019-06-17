
#ifndef __KERNEL_COMMAND_H
#define __KERNEL_COMMAND_H
#include "sys/std_int_h.h"

#define MAX_COMMAND_CHAR_LENGTH 100

struct command {
    char buf[MAX_COMMAND_CHAR_LENGTH];
    uint32_t current;

};

// extern struct command key_borard_command;
void append_char_to_command(char ch);
void key_borard_command_init();
void show_shell_root_pwd();
#endif

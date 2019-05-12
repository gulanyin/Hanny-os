#ifndef __DEVICE_CONSOLE_H
#define __DEVICE_CONSOLE_H


void entyr_console_init();
void console_acquire();
void console_release();

void console_str(char* );
void console_char(char ch);
void console_int_oct(int number);

#endif

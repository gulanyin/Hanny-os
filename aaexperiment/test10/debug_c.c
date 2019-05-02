#include "interrupt_h.h"
#include "debug_h.h"
#include "print_h.h"


/* 打印文件名,行号,函数名,条件并使程序悬停 */
void panic_spin(char* filename,
	           int line,
               const char* func,
		       const char* condition
           )
{
   interrupt_off();	//关中断。
   print_str("\n!!!!! error !!!!!\n");
   print_str("filename: ");print_str(filename);print_str("\n");
   print_str("line: ");print_int_oct(line);print_str("\n");
   print_str("function: ");print_str((char*)func);print_str("\n");
   print_str("condition: ");print_str((char*)condition);print_str("\n");
   while(1);
}

#include "kernel/command_h.h"
#include "kernel/string_h.h"
#include "kernel/print_h.h"
#include "file_system/fs_h.h"
#include "user/stdio_h.h"


// 汇编函数返回时间信息
extern int read_system_time(uint8_t port);


struct time_val{
	int sec;
	int min;
	int hour;
	int day;
	int mon;
	int year;
};


struct command key_borard_command;

void key_borard_command_init(){
    memset(key_borard_command.buf, 0, MAX_COMMAND_CHAR_LENGTH);
    key_borard_command.current = 0;
}



void show_shell_root_pwd(){
    char temp_buff_pwd[30] = {0};
    sys_getcwd(temp_buff_pwd, 30);
    printf("\nroot@root: %s# ", temp_buff_pwd);
}

void system_time(){

	struct time_val t;
	t.sec = read_system_time(0);
	t.min = read_system_time(2);
	t.hour = read_system_time(4);
	t.day = read_system_time(7);
	t.mon = read_system_time(8);
	t.year = read_system_time(9)+2000;
    printf("%u/%u/%u: %u:%u:%u", t.year, t.mon, t.day, t.hour, t.min, t.sec);
}

void append_char_to_command(char ch){
    if(ch != 13){
        key_borard_command.buf[key_borard_command.current] = ch;
        key_borard_command.current ++;
    }else{

        // 回车 解析命令

        if(!strcmp(key_borard_command.buf, "cls")){
            cls_screen();
        }else if (!strcmp(key_borard_command.buf, "pwd")){
                key_borard_command_init();
                sys_getcwd(key_borard_command.buf, MAX_COMMAND_CHAR_LENGTH);
                print_str(key_borard_command.buf);
        }else if (!strcmp(key_borard_command.buf, "ls")){
            // 打印文件或者目录信息
            struct stat obj_stat;
            int stat_index = 0;
            char* dirs[3] = {"/.dsf", "/dir1", "/file001"};
            for(stat_index=0; stat_index<3; stat_index++){
                sys_stat(dirs[stat_index], &obj_stat);
                printf("%s info   i_no:%d   size:%d   filetype:%s \n", dirs[stat_index], obj_stat.st_ino, obj_stat.st_size,  obj_stat.st_filetype == 2 ? "directory" : "regular");
            }
        }else if(!strcmp(key_borard_command.buf, "time")){
                system_time();
        }else{
            // key_borard_command.buf[key_borard_command.current] = '\n';
            // key_borard_command.current ++;
            printf("not support this command");
        }

        // 清空缓冲区
        key_borard_command_init();
        show_shell_root_pwd();
    }

}

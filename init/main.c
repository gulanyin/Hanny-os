#include "kernel/print_h.h"
#include "mm/init_memory_h.h"
#include "kernel/interrupt_h.h"
#include "thread/thread_h.h"
#include "device/console_h.h"
#include "userprog/tss_h.h"
#include "userprog/process_h.h"
#include "user/syscall_h.h"
#include "user/stdio_h.h"
#include "device/ide_h.h"
#include "device/timer_h.h"
#include "file_system/fs_h.h"
#include "file_system/dir_h.h"


void k_thread_a(void*);
void k_thread_b(void*);


void u_prog_a(void);
void u_prog_b(void);
int test_var_a = 1, test_var_b = 1;


void main(){

    // // 测试esp
    // uint32_t esp;
    // asm ("mov %%esp, %0" : "=g" (esp));
    // print_str("\n cccc esp:");print_int_oct(esp);

    // 内存管理
    entry_init_memory();
    // // test
    // 分配的第一个内核内存页，虚拟起始地址应该为 0xc000 0000 + 0x0020 2000  = > 0xc020 2000
    // uint32_t one_test_page = (uint32_t)get_kernel_pages(1);
    // print_str("\none_test_page virtual_addr_start:");print_int_oct(one_test_page);
    // while(1);


    // 线程管理
    entry_init_thread();

    // 初始化控制带锁
    entyr_console_init();


    thread_start("k_thread_a", 30, k_thread_a, "argA ");
    thread_start("k_thread_b", 10, k_thread_b, "argB ");



    // 初始化tss段
    entry_init_tss();




    // 中断设置
    entry_init_interrupt();

    console_str("entry_init_interrupt ok\n ");


    // 添加两个线程结构体
    process_execute(u_prog_a, "user_prog_a");
    process_execute(u_prog_b, "user_prog_b");
    console_str("process_execute ok\n");


    // 注册系统调用
    init_all_syscall();
    printk("ooook");




    // 开启中断
    on_interrupt();



    printk("main22 ");

    // 读取硬盘
    ide_init();
    filesys_init();   // 初始化文件系统
    // sys_open("/file002", O_CREAT);  // 创建一个文件
    //uint32_t fd = sys_open("/file002", O_RDONLY);  // 打开一个文件

    /*
    // uint32_t fd = sys_open("/file002", O_RDWR);  // 读写打开
    // printk("fd is : %u\n", fd);
    // sys_write(fd, "file002_content_hello\n", 22);
    */

    // uint32_t fd = sys_open("/file002", O_RDONLY);  // 打开一个文件
    // char temp_content[50] = {0};
    // sys_lseek(fd, 4, SEEK_SET);
    // sys_read(fd, temp_content, 45);
    // printf("content is : %s\n", temp_content);
    //
    //
    // sys_close(fd);
    // printk("fd : %u has closed \n", fd);

    // 删除文件
    // printf("/file002 delete %s!\n", sys_unlink("/file002") == 0 ? "success" : "fail");


    // 创建目录
    // printf("/dir1 create %s!\n", sys_mkdir("/dir1") == 0 ? "done" : "fail");

    // 打开目录
   //  struct dir* p_dir = sys_opendir("/dir1");
   //  if (p_dir) {
   //    printf("/dir1 open done!, inode_no is %u \n", p_dir->inode->i_no);
   //    if (sys_closedir(p_dir) == 0) {
   //       printf("/dir1 close done!\n");
   //    } else {
   //       printf("/dir1 close fail!\n");
   //    }
   // } else {
   //    printf("/dir1 open fail!\n");
   // }


   // // 遍历目录  /.dsf 是当前目录 /..dfw是上级目录
   // struct dir* p_dir = sys_opendir("/dir1");
   //  if (p_dir) {
   //    printf("/.dsf open done!, inode_no is %u \n", p_dir->inode->i_no);
   //    char* type = NULL;
   //    struct dir_entry* dir_e = NULL;
   //    while((dir_e = sys_readdir(p_dir))) {
   //  	 if (dir_e->f_type == FT_REGULAR) {
   //  	    type = "regular";
   //  	 } else {
   //  	    type = "directory";
   //  	 }
   //  	 printf("      %s   %s\n", type, dir_e->filename);
   //    }
   // } else {
   //    printf("/.dsf open fail!\n");
   // }


   // 删除目录
   // printf("try to delete nonempty directory /dir1\n");
   // printf("sys_rmdir: /dir1 delete %s!\n", sys_rmdir("/dir1") == -1 ? "failed": "success");


   // 线程当前运行工作目录
   // char temp_buff_pwd[30] = {0};
   // sys_getcwd(temp_buff_pwd, 30);
   // printf("cwd: %s!\n", temp_buff_pwd);
   // sys_chdir("/dir1");
   // sys_getcwd(temp_buff_pwd, 30);
   // printf("changed cwd: %s!\n", temp_buff_pwd);


   // 打印文件或者目录信息
   struct stat obj_stat;
   int stat_index = 0;
   char* dirs[3] = {"/", "/dir1", "/file001"};
   for(stat_index=0; stat_index<3; stat_index++){
       sys_stat(dirs[stat_index], &obj_stat);
       printf("%s info\n   i_no:%d\n   size:%d\n   filetype:%s\n", dirs[stat_index], obj_stat.st_ino, obj_stat.st_size,  obj_stat.st_filetype == 2 ? "directory" : "regular");
   }

    while(1) {
       asm volatile("hlt");
       //printk("main22 ");
    }

}



// k_thread_a 函数
void k_thread_a(void* args) {
   while(1) {


       // void* addr1 = sys_malloc(33);
       // sys_free(addr1);
       // void* addr2 = sys_malloc(33);
       // sys_free(addr2);
       // printk("%s addr1 %u addr2 %u", args, (uint32_t)addr1, (uint32_t)addr2);

      //console_str("test_var_a");
      //console_int_oct(test_var_a);
      printk("dfdaaa %x", 5);
      printk("%s", args);
      asm volatile("hlt");
      while(1) ;
   }
}

// k_thread_b 函数
void k_thread_b(void* args) {
    while(1) {
        //console_str(args);

        // void* addr = sys_malloc(33);
        // sys_free(addr);
        // printk("%s addr is %u\n", args, addr);


        //console_int_oct((int)addr);
       //console_str("test_var_b");
       //console_int_oct(test_var_b);
       printk("%s", args);
       asm volatile("hlt");
       while(1);
       // mtime_sleep(1000) ;
    }
}



// 测试用户进程
void u_prog_a(void) {

   while(1) {
     // test_var_a++;

     // printf("%s %c %x %d", "printf pid is", ' ',  getpid(), -10);
     // void* addr1 = malloc(33);
     // free(addr1);
     // void* addr2 = malloc(33);
     // free(addr2);
     // printf("addr1 %u addr2 %u", (int)addr1, (int)addr2);
     printf("u_prog_a");
    while(1);
   }
}

// 测试用户进程
void u_prog_b(void) {
    //getpid();
   while(1) {
      // test_var_b++;
      //printf("printf pid is : %x", getpid());
      // printf("%s %c %x %d", "printf pid is", ' ',  getpid(), -10);
      // void* addr1 = malloc(33);
      // void* addr2 = malloc(33);
      // free(addr1);
      // free(addr2);
      // printf("addr1 %u addr2 %u", (int)addr1, (int)addr2);

      printf("u_prog_b");
      while(1);
   }
}

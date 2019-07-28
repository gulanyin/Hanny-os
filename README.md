# Hanny-os

**Hanny-os, 汇编和c写的的一个简陋的操作系统内核**

## 1. 概述
Hanny-os, 是用汇编和c写的的一个非常非常简陋的操作系统内核。写这个内核的初衷是当时看一些内核书籍有些吃力，继而看了保护模式，为了深入了解操作系统内核，于是动手从引导扇区开始写起走。一步一步简单实现了以下几个功能：
- 内存管理
- 中断处理
- 线程进程切换
- 文件系统
- 系统调用
- 简单shell界面

## 2. 目录结构

     aaexperiment - - - - - -简单的几个实验
     boot - - - - - -  - - - 引导代码
	 device - - - - - -  - - 磁盘设备相关
	 file_system - - -  - -  文件系统
	 include - - - -  - - -  头文件
	 init  - -  - - - - - -  内核初始化入口
	 kernel  - - - - - -  -  内核相关代码
	 mm - - - - - - - -  - - 内存管理
	 output_result - - - - - make命令输出目录
	 thread - -  - - - - - - 线程
	 user - - - - - - - - -  标准输出和系统调用界面
	 userprog - - - -  - - - 用户级进程

## 3. 代码笔记
 见博客笔记 http://47.107.169.90:8443/type/12
## 4. 运行和调试
    环境：
    gcc (Ubuntu 8.3.0-6ubuntu1) 8.3.0 
    NASM version 2.13.02
    Bochs x86 Emulator 2.6.9


    编译代码：
    make clean && make
    
    运行：
    进入output_result
    cd output_result/
    bochs -f bochsrc-sample.bxrc -log bochsout.txt -q
    
    
    调试：
    bochsdbg -f bochsrc-sample.bxrc -log bochsout.txt -q
## 5. 运行效果图

   加载内核成功，初始化内存管理
   ![系统调用获取pid](http://47.107.169.90/images/2019-07-28/1564288684-a931b4cc-fa81-4add-af51-7b93f18cb2c4.png "系统调用获取pid")
   
   
   三个内核线程切换：
   ![线程切换](http://47.107.169.90/images/2019-07-26/1564101717-c541a8e6-3810-447f-9948-53ecfdca0570.png "线程切换")
   
   
   用户进程的线程切换：
   k_thread_a、k_thread_b、main是内核线程。
   user_prog_a、user_prog_b是进程。
   ![用户进程的线程切换](http://47.107.169.90/images/2019-07-27/1564236501-fb6e34b3-578c-4f7c-8da5-3ef91cd31bbe.png "用户进程的线程切换")
   
   
   系统调用获取pid：
   ![系统调用获取pid](http://47.107.169.90/images/2019-07-27/1564240791-0e015615-1416-4534-87f5-548ba901970d.png "系统调用获取pid")
   
   
   磁盘及分区信息：
   ![系统调用获取pid](http://47.107.169.90/images/2019-07-28/1564289044-fcc85875-0521-4a57-964a-0337ff6dbcef.png "系统调用获取pid")
   
   
   shell命令
   ![系统调用获取pid](http://47.107.169.90/images/2019-07-28/1564289438-3fb295b2-d868-4463-bac8-6fc352debd71.png "系统调用获取pid")


## 6. 调试过程
   有时修改了单个文件后，make时有可能会造成链接地址不一致，应清理下再make
   
   调试：
   调试过程中只能查看到汇编代码，对于用汇编写的很容易能找到对应的代码，而c语言写的函数则通过
   cat objdump -d output/boot/system.elf 
   查看汇编代码来定位。

## 7. 未完后续...
  写此代码时主要是为了深刻地理解操作系统内核，因此有些地方代码格式命名等也比较凌乱，写的也很简陋。
  后期：
  1. 重构代码
  2. 增加网络相关
  3. 增加用户界面
  4. 其他更多...

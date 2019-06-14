#ifndef __FILE_SYSTEM_INODE_H
#define __FILE_SYSTEM_INODE_H
#include "sys/std_int_h.h"
#include "kernel/list_h.h"
#include "sys/gloab_h.h"

/* inode结构 */
struct inode {
   uint32_t i_no;    // inode编号

/* 当此inode是文件时,i_size是指文件大小,
若此inode是目录,i_size是指该目录下所有目录项大小之和*/
   uint32_t i_size;

   uint32_t i_open_cnts;   // 记录此文件被打开的次数
   bool write_deny;	   // 写文件不能并行,进程写文件前检查此标识

/* i_sectors[0-11]是直接块, i_sectors[12]用来存储一级间接块指针 */
   uint32_t i_sectors[13];
   LIST_ELEM inode_tag;
};
#endif

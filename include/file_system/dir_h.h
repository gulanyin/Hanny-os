#ifndef __FILE_SYSTEM_DIR_H
#define __FILE_SYSTEM_DIR_H

#include "sys/std_int_h.h"
#include "file_system/inode_h.h"
#include "file_system/fs_h.h"
#include "sys/gloab_h.h"
#include "device/ide_h.h"



#define MAX_FILE_NAME_LEN  16	 // 最大文件名长度

/* 目录结构 */
struct dir {
   struct inode* inode;
   uint32_t dir_pos;	  // 记录在目录内的偏移
   uint8_t dir_buf[512];  // 目录的数据缓存
};

/* 目录项结构 */
struct dir_entry {
   char filename[MAX_FILE_NAME_LEN];  // 普通文件或目录名称
   uint32_t i_no;		      // 普通文件或目录对应的inode编号
   enum file_types f_type;	      // 文件类型
};

extern struct dir root_dir;             // 根目录
void create_dir_entry(char* filename, uint32_t inode_no, uint8_t file_type, struct dir_entry* p_de);
bool sync_dir_entry(struct dir* parent_dir, struct dir_entry* p_de, void* io_buf) ;

void dir_close(struct dir* dir);
struct dir* dir_open(struct partition* part, uint32_t inode_no);
bool search_dir_entry(struct partition* part, struct dir* pdir, const char* name, struct dir_entry* dir_e);
void open_root_dir(struct partition* part);
#endif

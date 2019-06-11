#ifndef __DEVICE_IDE_H
#define __DEVICE_IDE_H
#include "sys/std_int_h.h"
#include "kernel/sync_h.h"
#include "kernel/bitmap_h.h"
#include "kernel/list_h.h"
#include "sys/gloab_h.h"


/* 分区结构 */
struct partition {
   uint32_t start_lba;		 // 起始扇区
   uint32_t sec_cnt;		 // 扇区数
   struct disk* my_disk;	 // 分区所属的硬盘
   LIST_ELEM part_tag;	 // 用于队列中的标记
   char name[8];		 // 分区名称
   struct super_block* sb;	 // 本分区的超级块
   BITMAP block_bitmap;	 // 块位图
   BITMAP inode_bitmap;	 // i结点位图
   LIST open_inodes;	 // 本分区打开的i结点队列
};

/* 硬盘结构 */
struct disk {
   char name[8];			   // 本硬盘的名称，如sda等
   struct ide_channel* my_channel;	   // 此块硬盘归属于哪个ide通道
   uint8_t dev_no;			   // 本硬盘是主0还是从1
   struct partition prim_parts[4];	   // 主分区顶多是4个
   struct partition logic_parts[8];	   // 逻辑分区数量8个,
};



// ata通道结构
struct ide_channel {
   char name[8];		 // 本ata通道名称
   uint16_t port_base;		 // 本通道的起始端口号
   uint8_t irq_no;		 // 本通道所用的中断号
   LOCK lock;		 // 通道锁
   bool expecting_intr;		 // 表示等待硬盘的中断
   SEMAPHORE disk_done;	 // 用于阻塞、唤醒驱动程序
   struct disk devices[2];	 // 一个通道上连接两个硬盘，一主一从
};











void ide_init(void);
extern uint8_t channel_cnt;
extern struct ide_channel channels[];
#endif

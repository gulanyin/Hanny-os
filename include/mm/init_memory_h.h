#ifndef __MM_INIT_MERMORY_H
#define __MM_INIT_MERMORY_H

#include "sys/std_int_h.h"
#include "kernel/bitmap_h.h"
#include "kernel/sync_h.h"

#define	 PG_P_1	  1	// 页表项或页目录项存在属性位
#define	 PG_P_0	  0	// 页表项或页目录项存在属性位
#define	 PG_RW_R  0	// R/W 属性位值, 读/执行
#define	 PG_RW_W  2	// R/W 属性位值, 读/写/执行
#define	 PG_US_S  0	// U/S 属性位值, 系统级
#define	 PG_US_U  4	// U/S 属性位值, 用户级

// 虚拟地址
// typedef struct _VIRTUAL_ADDRESS{
//     BITMAP virtual_addr_bitmap;
//     uint32_t virtual_addr_start;
// } VIRTUAL_ADDRESS, *PVIRTUAL_ADDRESS;



// 内存池
typedef struct _MEMORY_POOL{
    BITMAP pool_bitmap;
    uint32_t pool_physical_address_start;
    uint32_t pool_size;  // bytes length
    LOCK lock;
} MEMORY_POOL, *PMEMORY_POOL;


extern MEMORY_POOL  memory_pool_kernel, memory_pool_user;
void entry_init_memory();
void* get_kernel_pages(uint32_t page_cnt);
uint32_t addr_v2p(uint32_t vaddr);
void* get_a_page(int pf_type, uint32_t virtual_address);
void* sys_malloc(uint32_t size);
void sys_free(void* ptr) ;

#endif

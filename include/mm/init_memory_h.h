#ifndef __MM_INIT_MERMORY_H
#define __MM_INIT_MERMORY_H

#include "sys/std_int_h.h"
#include "kernel/bitmap_h.h"

// 虚拟地址
typedef struct _VIRTUAL_ADDRESS{
    BITMAP virtual_addr_bitmap;
    uint32_t virtual_addr_start;
} VIRTUAL_ADDRESS, *PVIRTUAL_ADDRESS;



// 内存池
typedef struct _MEMORY_POOL{
    BITMAP pool_bitmap;
    uint32_t pool_physical_address_start;
    uint32_t pool_size;  // bytes length
} MEMORY_POOL, *PMEMORY_POOL;


extern MEMORY_POOL  memory_pool_kernel, memory_pool_user;
void init_memory();
void* get_kernel_pages(uint32_t page_cnt);

#endif

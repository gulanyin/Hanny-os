#ifndef __MM_MERMORY_H
#define __MM_MERMORY_H

#include "sys/std_int_h.h"
#include "kernel/bitmap_h.h"


// 虚拟地址
typedef struct _VIRTUAL_ADDRESS{
    BITMAP virtual_addr_bitmap;
    uint32_t virtual_addr_start;
} VIRTUAL_ADDRESS, *PVIRTUAL_ADDRESS;

#endif

#ifndef __MM_MEM_DESC_H
#define __MM_MEM_DESC_H

#include "kernel/list_h.h"
#include "sys/gloab_h.h"

#define DESC_COUNT 7


struct mem_block{
    LIST_ELEM free_elem;
};

struct mem_block_desc{
    uint32_t block_size;
    uint32_t blocks_per_arena;
    LIST free_list;
};




struct arena{
    struct mem_block_desc * desc;
    uint32_t cnt;
    bool large;
};


void block_desc_init(struct mem_block_desc* desc_array);

#endif

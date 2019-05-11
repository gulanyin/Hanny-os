#ifndef __KERNEL_BITMAP_H
#define __KERNEL_BITMAP_H
#include "sys/std_int_h.h"

typedef struct _BITMAP {
    uint32_t bytes_length;
    uint8_t* start_addr;
} BITMAP, *PBITMAP;


void bitmap_init(PBITMAP bitmap);
unsigned char bitmap_scan_test(PBITMAP p_bitmap, uint32_t bit_index);
int bitmap_scan(PBITMAP btmp, uint32_t cnt);
void bitmap_set(PBITMAP btmp, uint32_t bit_idx, int8_t value);
#endif

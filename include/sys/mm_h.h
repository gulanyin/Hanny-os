#ifndef __SYS_MM_H
#define __SYS_MM_H

#include "std_int_h.h"

typedef struct _MEMORY_DESCRIPTOR {
    unsigned int add_low;
    unsigned int add_hig;
    unsigned int len_low;
    unsigned int len_hig;
    unsigned int type;
} MEMORY_DESCRIPTOR, *PMEMORY_DESCRIPTOR;





#endif

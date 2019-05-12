#ifndef __KERNEL_LIST_H
#define __KERNEL_LIST_H

#include "sys/gloab_h.h"
#include "sys/std_int_h.h"

// 链表上的节点
struct list_elem {
   struct list_elem* prev; // 前躯结点
   struct list_elem* next; // 后继结点
};

typedef struct list_elem LIST_ELEM, *PLIST_ELEM;
// typedef struct list_elem* PLIST_ELEM;
// typedef struct _LIST_ELEM {
//     PLIST_ELEM prev;
//     PLIST_ELEM next;
//
// }LIST_ELEM, *PLIST_ELEM;


// 队列链表
typedef struct _LIST {
    LIST_ELEM head;
    LIST_ELEM tail;
} LIST, *PLIST;


typedef bool (function)(PLIST_ELEM p_list_elem, int arg);


void init_list(PLIST p_list);
void list_insert_before(PLIST_ELEM p_before, PLIST_ELEM  p_elem);
void list_push(PLIST p_list, PLIST_ELEM  p_elem);
void list_append(PLIST p_list, PLIST_ELEM  p_elem);
void list_remove(PLIST_ELEM  p_elem);
PLIST_ELEM list_pop(PLIST p_list);
bool list_empty(PLIST plist);
bool elem_find(PLIST p_list, PLIST_ELEM  obj_elem);
PLIST_ELEM list_traversal(PLIST plist, function func, int arg);
uint32_t list_len(PLIST plist);
#endif

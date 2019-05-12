#include "kernel/list_h.h"
#include "kernel/interrupt_h.h"
#include "sys/gloab_h.h"
#include "sys/std_int_h.h"



// // 链表上的节点
// typedef struct _LIST_ELEM {
//     PLIST_ELEM prev;
//     PLIST_ELEM next;
//
// }LIST_ELEM, *PLIST_ELEM;
//
//
// // 队列链表
// typedef struct _LIST {
//     LIST_ELEM head;
//     LIST_ELEM tail;
// } LIST, PLIST;


// 初始化双向链表list
void init_list(PLIST p_list){
    p_list->head.prev = NULL;
    p_list->head.next = &p_list->tail;
    p_list->tail.prev = &p_list->head;
    p_list->tail.next = NULL;
}


// before 之前插入 elem
void list_insert_before(PLIST_ELEM p_before, PLIST_ELEM  p_elem){

    // 初始化线程的时候还没有设置好中断相关的东西，不能简单直接地开关中断
    // off_interrupt();
    enum interrupt_status old_status = interrupt_disable();

    p_elem->next = p_before;
    p_elem->prev = p_before->prev;
    p_before->prev->next = p_elem;
    p_before->prev = p_elem;

    // on_interrupt();
    set_interrupt_status(old_status);
}

// 添加元素到队列头部
void list_push(PLIST p_list, PLIST_ELEM  p_elem){
    list_insert_before(p_list->head.next, p_elem);
}

// 添加元素到队列尾部
void list_append(PLIST p_list, PLIST_ELEM  p_elem){
    list_insert_before(&p_list->tail, p_elem);
}


// 删除一个元素
void list_remove(PLIST_ELEM  p_elem){
    // off_interrupt();
    enum interrupt_status old_status = interrupt_disable();

    p_elem->prev->next = p_elem->next;
    p_elem->next->prev = p_elem->prev;
    // on_interrupt();
    set_interrupt_status(old_status);
}


// pop 弹出队首元素
PLIST_ELEM list_pop(PLIST p_list){
    PLIST_ELEM  p_elem = p_list->head.next;
    list_remove(p_elem);
    return p_elem;
}


// 判断链表是否为空,空时返回true,否则返回false
bool list_empty(PLIST plist) {		// 判断队列是否为空
   return (plist->head.next == &plist->tail ? true : false);
}

// 从链表中查找元素obj_elem,成功时返回true,失败时返回false
bool elem_find(PLIST p_list, PLIST_ELEM  obj_elem){
    PLIST_ELEM p_elem = p_list->head.next;
    while (p_elem != &p_list->tail) {
        if (p_elem == obj_elem) {
           return true;
        }
       p_elem = p_elem->next;
   }
   return false;
}


PLIST_ELEM list_traversal(PLIST plist, function func, int arg) {
   PLIST_ELEM elem = plist->head.next;
   if (list_empty(plist)) {
      return NULL;
   }

   while (elem != &plist->tail) {
      if (func(elem, arg)) {		  // func返回ture则认为该元素在回调函数中符合条件,命中,故停止继续遍历
	         return elem;
      }					  // 若回调函数func返回true,则继续遍历
      elem = elem->next;
   }
   return NULL;
}


// 返回链表长度
uint32_t list_len(PLIST plist) {
   PLIST_ELEM elem = plist->head.next;
   uint32_t length = 0;
   while (elem != &plist->tail) {
      length++;
      elem = elem->next;
   }
   return length;
}

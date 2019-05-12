#include "sys/mm_h.h"

#include "kernel/print_h.h"
#include "kernel/debug_h.h"
#include "kernel/string_h.h"
#include "kernel/bitmap_h.h"
#include "mm/init_memory_h.h"

#define PAGE_SIZE 4096
#define PG_ATTRIBUTE_S_R_W_P 3   // u/s = 0, r/w=1, p=1
#define PDE_INDEX(addr) ((addr & 0xffc00000) >> 22)
#define PTE_INDEX(addr) ((addr & 0x003ff000) >> 12)

#define	 PG_P_1	  1	// 页表项或页目录项存在属性位
#define	 PG_P_0	  0	// 页表项或页目录项存在属性位
#define	 PG_RW_R  0	// R/W 属性位值, 读/执行
#define	 PG_RW_W  2	// R/W 属性位值, 读/写/执行
#define	 PG_US_S  0	// U/S 属性位值, 系统级
#define	 PG_US_U  4	// U/S 属性位值, 用户级

/* 内存池标记,用于判断用哪个内存池 */
enum pool_flags {
   PF_KERNEL = 1,    // 内核内存池
   PF_USER = 2	     // 用户内存池
};




MEMORY_POOL kernel_pool, user_pool;
VIRTUAL_ADDRESS kernel_virtual;




// 在 p_pool 指向的物理内存池中分配1个物理页,成功则返回页框的物理地址,失败则返回NULL
static void* alloc_memory(PMEMORY_POOL p_pool) {
   // 扫描或设置位图要保证原子操作
   int bit_idx = bitmap_scan(&p_pool->pool_bitmap, 1);
   // 找一个物理页面
   if (bit_idx == -1 ) {
      return NULL;
   }
   bitmap_set(&p_pool->pool_bitmap, bit_idx, 1);	// 将此位bit_idx置1
   uint32_t page_physical_address = ((bit_idx * PAGE_SIZE) + p_pool->pool_physical_address_start);
   return (void*)page_physical_address;
}


// 虚拟地址vaddr对应的pte的虚拟地址
uint32_t* pte_virtual_address(uint32_t vaddr) {
    // 先访问到页表自己 + \
    // 再用页目录项pde(页目录内页表的索引)做为pte的索引访问到页表 + \
    // 再用pte的索引做为页内偏移
   uint32_t* pte = (uint32_t*)(0xffc00000 + \
	 ((vaddr & 0xffc00000) >> 10) + \
	 PTE_INDEX(vaddr) * 4);
   return pte;
}

// 虚拟地址vaddr对应的pde的虚拟地址
uint32_t* pde_virtual_address(uint32_t vaddr) {
   // 0xfffff是用来访问到页表本身所在的地址
   uint32_t* pde = (uint32_t*)((0xfffff000) + PDE_INDEX(vaddr) * 4);
   return pde;
}



// 页表中添加虚拟地址virtual_address与物理地址physical_address的映射 */
static void page_table_add(void* _virtual_address, void* _physical_address) {
   uint32_t virtual_address = (uint32_t)_virtual_address, physical_address = (uint32_t)_physical_address;
   uint32_t* pde = pde_virtual_address(virtual_address);
   uint32_t* pte = pte_virtual_address(virtual_address);

   /************************   注意   *************************
    *
    * 执行*pte,会访问到空的pde。所以确保pde创建完成后才能执行*pte,
    * 否则会引发page_fault。因此在*pde为0时,*pte只能出现在下面else语句块中的*pde后面。
    *
    *********************************************************/
    //先在页目录内判断目录项的P位，若为1,则表示该表已存在
    // 页目录项和页表项的第0位为P,此处判断目录项是否存在
   if (*pde & 0x00000001) {
      ASSERT(!(*pte & 0x00000001));

      if (!(*pte & 0x00000001)) {
          // 只要是创建页表,pte就应该不存在,多判断一下放心
          // US=1,RW=1,P=1
         *pte = (physical_address | PG_US_U | PG_RW_W | PG_P_1);
      } else {
          //应该不会执行到这，因为上面的ASSERT会先执行。
          PANIC("pte repeat");
    	 *pte = (physical_address | PG_US_U | PG_RW_W | PG_P_1);      // US=1,RW=1,P=1
      }
   } else {
       // 页目录项不存在,所以要先创建页目录再创建页表项.
      // 页表中用到的页框一律从内核空间分配
      uint32_t pde_phyaddr = (uint32_t)alloc_memory(&kernel_pool);

      *pde = (physical_address | PG_US_U | PG_RW_W | PG_P_1);

      /* 分配到的物理页地址pde_phyaddr对应的物理内存清0,
       * 避免里面的陈旧数据变成了页表项,从而让页表混乱.
       * 访问到pde对应的物理地址,用pte取高20位便可.
       * 因为pte是基于该pde对应的物理地址内再寻址,
       * 把低12位置0便是该pde对应的物理页的起始*/
      memset((void*)((int)pte & 0xfffff000), 0, PAGE_SIZE);

      ASSERT(!(*pte & 0x00000001));
      *pte = (physical_address | PG_US_U | PG_RW_W | PG_P_1);      // US=1,RW=1,P=1
   }
}



// 在pf表示的虚拟内存池中申请pg_cnt个虚拟页,成功则返回虚拟页的起始地址, 失败则返回NULL
static void* virtual_address_get(enum pool_flags pf, uint32_t page_count) {
   int vaddr_start = 0, bit_idx_start = -1;
   uint32_t cnt = 0;
   if (pf == PF_KERNEL) {
      bit_idx_start  = bitmap_scan(&kernel_virtual.virtual_addr_bitmap, page_count);
      if (bit_idx_start == -1) {
          return NULL;
      }
      while(cnt < page_count) {
          bitmap_set(&kernel_virtual.virtual_addr_bitmap, bit_idx_start + cnt++, 1);
      }
      vaddr_start = kernel_virtual.virtual_addr_start + bit_idx_start * PAGE_SIZE;
   } else {
       // 用户内存池,将来实现用户进程再补充
   }
   return (void*)vaddr_start;
}



// 分配 page_count个页空间,成功则返回起始虚拟地址,失败时返回NULL
void* malloc_page(enum pool_flags pf, uint32_t page_count) {

    ASSERT(page_count > 0 && page_count < 3840);
/***********   malloc_page的原理是三个动作的合成:   ***********
      1通过vaddr_get在虚拟内存池中申请虚拟地址
      2通过palloc在物理内存池中申请物理页
      3通过page_table_add将以上得到的虚拟地址和物理地址在页表中完成映射
***************************************************************/
   void* virtual_address_start = virtual_address_get(pf, page_count);
   if (virtual_address_start == NULL) {
      return NULL;
   }

   uint32_t vaddr = (uint32_t)virtual_address_start, cnt = page_count;
   PMEMORY_POOL mem_pool = pf & PF_KERNEL ? &kernel_pool : &user_pool;

   /* 因为虚拟地址是连续的,但物理地址可以是不连续的,所以逐个做映射*/
   while (cnt-- > 0) {
      void* page_phyaddr = alloc_memory(mem_pool);
      if (page_phyaddr == NULL) {  // 失败时要将曾经已申请的虚拟地址和物理页全部回滚，在将来完成内存回收时再补充
	         return NULL;
      }
      page_table_add((void*)vaddr, page_phyaddr); // 在页表中做映射
      vaddr += PAGE_SIZE;		 // 下一个虚拟页
   }
   return virtual_address_start;
}



//从内核物理内存池中申请pg_cnt页内存,成功则返回其虚拟地址,失败则返回NULL
void* get_kernel_pages(uint32_t pg_cnt) {
   void* vaddr =  malloc_page(PF_KERNEL, pg_cnt);
   if (vaddr != NULL) {	   // 若分配的地址不为空,将页框清0后返回
      memset(vaddr, 0, pg_cnt * PAGE_SIZE);
   }
   return vaddr;
}




void init_pool(unsigned int all_mem){
    int i = 0;
    // 总内存大小，起始位置为1M
    uint32_t page_table_size = PAGE_SIZE * 2;  // 从2*1024*1024起分配了连续两个页面，第一个为页目录表，第二个为页表
    // 起始可用内存是从1*1024*1024 起开始算的，
    uint32_t used_mem = 0x100000 + page_table_size;  // 1M起的1M加上两个页面的空间已被占用
    uint32_t free_mem = all_mem - used_mem;
    uint16_t all_free_pages = free_mem / PAGE_SIZE;
    uint16_t kernel_free_pages = all_free_pages / 2;
    uint16_t user_free_pages = all_free_pages - kernel_free_pages;

    uint32_t kerner_start_addr = 0x100000 + used_mem  ;
    uint32_t user_start_addr = kerner_start_addr + kernel_free_pages * PAGE_SIZE;

    kernel_pool.pool_size = kernel_free_pages * PAGE_SIZE;
    user_pool.pool_size = user_free_pages * PAGE_SIZE;

    kernel_pool.pool_physical_address_start = kerner_start_addr;
    user_pool.pool_physical_address_start = user_start_addr;

    kernel_pool.pool_bitmap.bytes_length = kernel_free_pages / 8;
    user_pool.pool_bitmap.bytes_length = user_free_pages / 8;

    kernel_pool.pool_bitmap.start_addr = (void*) (1 * 1024 * 1024); // 内核内存池的位图从1M的地方开始放
    user_pool.pool_bitmap.start_addr = (void*) (1 * 1024 * 1024 + kernel_free_pages / 8);

    // 虚拟内核
    kernel_virtual.virtual_addr_bitmap.bytes_length = kernel_free_pages / 8;
    kernel_virtual.virtual_addr_bitmap.start_addr = (void*)(1 * 1024 * 1024 + kernel_free_pages / 8 + user_free_pages / 8);
    kernel_virtual.virtual_addr_start = 0xc0000000 + kerner_start_addr; // 虚拟地址映射，


    bitmap_init(&kernel_pool.pool_bitmap);
    bitmap_init(&user_pool.pool_bitmap);
    bitmap_init(&kernel_virtual.virtual_addr_bitmap);

    print_str("\nkernel_pool_bitmap_start ");print_int_oct((unsigned int)kernel_pool.pool_bitmap.start_addr);
    print_str("\nkernel_pool_bitmap_ len ");print_int_oct((unsigned int)kernel_pool.pool_bitmap.bytes_length);
    print_str("\nkernel_pool_phy_addr_start:");print_int_oct(kernel_pool.pool_physical_address_start);
    print_str("\nkernel_pool length:");print_int_oct(kernel_pool.pool_size);

    print_str("\nuser_pool_bitmap_start ");print_int_oct((unsigned int)user_pool.pool_bitmap.start_addr);
    print_str("\nuser_pool_bitmap len ");print_int_oct((unsigned int)user_pool.pool_bitmap.bytes_length);
    print_str("\nuser_pool_phy_addr_start:");print_int_oct(user_pool.pool_physical_address_start);
    print_str("\nuser_pool length:");print_int_oct(user_pool.pool_size);

    print_str("\nkernel_virtual virtual_addr_bitmap.start_addr: ");print_int_oct((unsigned int)kernel_virtual.virtual_addr_bitmap.start_addr);
    print_str("\nkernel_virtual virtual_addr_start:");print_int_oct(kernel_virtual.virtual_addr_start);

    print_str("\nmemory pool init ok!");


    // 内存管理结构初始化好了，初始化页目录和页表
    unsigned int * p_pde_start = (unsigned int *) (2*1024*1024);
    unsigned int * p_first_pte_start = (unsigned int *) (2*1024*1024 + PAGE_SIZE);

    // 第一个页目录项指向第一个页表,768页目录项指向内核高1G空间
    *(p_pde_start + 0) = (  (2*1024*1024 + 1*PAGE_SIZE)  | PG_ATTRIBUTE_S_R_W_P);
    *(p_pde_start + 768) =  (  (2*1024*1024 + 1*PAGE_SIZE)  | PG_ATTRIBUTE_S_R_W_P);
    *(p_pde_start + 1023) =  (  2*1024*1024  | PG_ATTRIBUTE_S_R_W_P);
    // 第一个页表项指向物理内存0处， 一个页表能指向1k* 4k = 4M内存, 内核代码从内存0起始大约几十kb
    // 前512个页表项指向 0-2M的物理内存
    *p_first_pte_start = 0x0;
    for(i=0; i<256*2+2; i++){  // 2M零2个页面的内存
        *(p_first_pte_start + i) = (  (PAGE_SIZE * i)  ) | PG_ATTRIBUTE_S_R_W_P;
    }


    // 开启分页
    // 设置cr3
    asm volatile ( "push %eax");
    asm volatile ( "mov $0x00200000, %eax");
    asm volatile ( "mov %eax, %cr3");
    print_str("\n cr3 ok");

    //即将CR0寄存器的第31位置1
    asm volatile ( "mov %cr0, %eax");
    asm volatile ( "or $0x80000000, %eax");
    asm volatile ( "mov %eax, %cr0");
    asm volatile ( "pop %eax");
    print_str("\n pagenation ok ");


    // while(1);

}




void entry_init_memory(){
    // 根据存放在0x85000处的内存信息初始化内存管理
    int i = 0;
    int max_access_addr = 0; // 最大能访问到的内存, 最大不超过4GB
    int has_useable = 0; // 从1m开始的内存为可用内存，且可用内存长度不低于32M

    print_char('\n');
    print_str("init memory start");


    int mem_number = *((int*)(0x85000));
    PMEMORY_DESCRIPTOR p_mm = (PMEMORY_DESCRIPTOR)0x85010;


    print_int_oct(       mem_number     );
    print_char('\n');

    for(i=0; i<mem_number; i++){
        // 找出可用内存能访问的最大地址
        print_str("addr-high ");
        print_int_oct(       (p_mm + i)->add_hig    );
        print_str("addr-low ");
        print_int_oct(       (p_mm + i)->add_low     );
        print_str("   len-hig ");
        print_int_oct(       (p_mm + i)->len_hig     );
        print_str("   len_low ");
        print_int_oct(       (p_mm + i)->len_low     );
        print_str("   type ");
        print_int_oct(       (p_mm + i)->type     );
        print_char('\n');
    }

    // 获取内存信息失败
    if(mem_number == 0){
        print_str("mem_number is 0, init memory failed");
        while(1);
    }


    for(i=0; i<mem_number; i++){
        if( (p_mm + i)->type  == 1  &&
            (p_mm + i)->add_low == 1*1024*1024 &&
            (p_mm + i)->len_low > 32*1024*1024
        ){
            has_useable = 1;
            break;
        }
    }

    if(has_useable){
        print_str("useable memory: addr=> ");
        print_int_oct((p_mm + i)->add_low);
        print_str("  length=> ");
        print_int_oct((p_mm + i)->len_low);
    }else{
        print_str("don't have useable memory");
         while(1);
    }




    init_pool((p_mm + i)->len_low);


    print_char('\n');
    print_str("init memory end");

    // while(1);

}

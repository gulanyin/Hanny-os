#include "kernel/bitmap_h.h"
#include "sys/std_int_h.h"
#include "kernel/string_h.h"
#include "kernel/debug_h.h"


void bitmap_init(PBITMAP bitmap){
    memset(bitmap->start_addr, 0, bitmap->bytes_length);
}

unsigned char bitmap_scan_test(PBITMAP p_bitmap, uint32_t bit_index){
    uint32_t byte_index = bit_index / 8; // 位所在字节
    uint32_t bit_odd = bit_index % 8;  // 偏移
    return (p_bitmap->start_addr[byte_index] & (1 << bit_odd));
}




// 在位图中申请连续cnt个位,成功则返回其起始位下标，失败返回-1
int bitmap_scan(PBITMAP btmp, uint32_t cnt) {
   uint32_t idx_byte = 0;	 // 用于记录空闲位所在的字节
    // 先逐字节比较
   while (( 0xff == btmp->start_addr[idx_byte]) && (idx_byte < btmp->bytes_length)) {
      // 1表示该位已分配,所以若为0xff,则表示该字节内已无空闲位,向下一字节继续找
      idx_byte++;
   }

   ASSERT(idx_byte < btmp->bytes_length);
   if (idx_byte == btmp->bytes_length) {  // 若该内存池找不到可用空间
      return -1;
   }

 /* 若在位图数组范围内的某字节内找到了空闲位，
  * 在该字节内逐位比对,返回空闲位的索引。*/
   int idx_bit = 0;
 /* 和btmp->bits[idx_byte]这个字节逐位对比 */
   while ((uint8_t)(1 << idx_bit) & btmp->start_addr[idx_byte]) {
	 idx_bit++;
   }

   int bit_idx_start = idx_byte * 8 + idx_bit;    // 空闲位在位图内的下标
   if (cnt == 1) {
      return bit_idx_start;
   }

   uint32_t bit_left = (btmp->bytes_length * 8 - bit_idx_start);   // 记录还有多少位可以判断
   uint32_t next_bit = bit_idx_start + 1;
   uint32_t count = 1;	      // 用于记录找到的空闲位的个数

   bit_idx_start = -1;	      // 先将其置为-1,若找不到连续的位就直接返回
   while (bit_left-- > 0) {
      if (!(bitmap_scan_test(btmp, next_bit))) {	 // 若next_bit为0
	         count++;
      } else {
	         count = 0;
      }
      if (count == cnt) {	    // 若找到连续的cnt个空位
	         bit_idx_start = next_bit - cnt + 1;
	            break;
      }
      next_bit++;
   }
   return bit_idx_start;
}



/* 将位图btmp的bit_idx位设置为value */
void bitmap_set(PBITMAP btmp, uint32_t bit_idx, int8_t value) {
   ASSERT((value == 0) || (value == 1));
   uint32_t byte_idx = bit_idx / 8;    // 向下取整用于索引数组下标
   uint32_t bit_odd  = bit_idx % 8;    // 取余用于索引数组内的位

/* 一般都会用个0x1这样的数对字节中的位操作,
 * 将1任意移动后再取反,或者先取反再移位,可用来对位置0操作。*/
   if (value) {		      // 如果value为1
      btmp->start_addr[byte_idx] |= (1 << bit_odd);
   } else {		      // 若为0
      btmp->start_addr[byte_idx] &= ~(1 << bit_odd);
   }
}

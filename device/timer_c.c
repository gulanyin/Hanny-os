#include "sys/std_int_h.h"
#include "thread/thread_h.h"
#include "kernel/debug_h.h"

#define IRQ0_FREQUENCY	   100
#define mil_seconds_per_intr (1000 / IRQ0_FREQUENCY)
#define DIV_ROUND_UP(X, STEP) ((X + STEP - 1) / (STEP))

extern uint32_t kernel_ticks;  // 内核开启中断依赖滴答数



// 以tick为单位的sleep,任何时间形式的sleep会转换此ticks形式
static void ticks_to_sleep(uint32_t sleep_ticks) {
   uint32_t start_tick = kernel_ticks;

   // 若间隔的ticks数不够便让出cpu
   while ((kernel_ticks - start_tick) < sleep_ticks) {
      thread_yield();
   }
}


/* 以毫秒为单位的sleep   1秒= 1000毫秒 */
void mtime_sleep(uint32_t m_seconds) {
  uint32_t sleep_ticks = DIV_ROUND_UP(m_seconds, mil_seconds_per_intr);
  ASSERT(sleep_ticks > 0);
  ticks_to_sleep(sleep_ticks);
}

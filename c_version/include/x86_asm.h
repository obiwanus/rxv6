#include "base.h"

static inline u8 in_u8(u16 port) {
  u16 data;
  __asm__ volatile("in %[port], %[data]" : [data] "=a"(data) : [port] "d"(port));
  return data;
}

static inline void in_u32_array(u16 port, void *addr, int count) {
  __asm__ volatile("cld; rep insl"
                   : "=D"(addr), "=c"(count)
                   : "d"(port), "0"(addr), "1"(count)
                   : "memory", "cc");
}

static inline void out_u8(u16 port, u8 data) {
  __asm__ volatile("out %[data], %[port]" : : [data] "a"(data), [port] "d"(port));
}

static inline void stosb(void *addr, u8 data, int count) {
  __asm__ volatile("cld; rep stosb"
                   : "=D"(addr), "=c"(count)
                   : "0"(addr), "1"(count), "a"(data)
                   : "memory", "cc");
}

static inline void load_cr3(void *addr) {
  __asm__ volatile("movl %[page_table], %%cr3" : : [page_table] "r"(addr));
}

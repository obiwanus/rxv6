#include "base.h"

static inline u8 in_u8(u16 port) {
  u16 data;
  asm volatile("in %[port], %[data]" : [data] "=a"(data) : [port] "d"(port));
  return data;
}

static inline void in_u32_array(u16 port, void *addr, int count) {
  asm volatile("cld; rep insl"
               : "=D"(addr), "=c"(count)
               : "d"(port), "0"(addr), "1"(count)
               : "memory", "cc");
}

static inline void out_u8(u16 port, u8 data) {
  asm volatile("out %[data], %[port]" : : [data] "a"(data), [port] "d"(port));
}

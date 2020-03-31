static inline u8 in_u8(u16 port) {
  u16 data;
  asm volatile("in %1, %0" : "=a"(data) : "d"(port));
  return data;
}

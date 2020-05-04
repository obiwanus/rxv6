#include "x86_asm.h"

#include "string.h"

void
memset(void *va, u8 pattern, int len)
{
  if ((u32)va % 4 == 0 && len % 4 == 0) {
    // Store by dword
    u32 dw = (u32)pattern;
    store_u32s(va, (dw << 24) | (dw << 16) | (dw << 8) | dw, len / 4);
  } else {
    store_u8s(va, pattern, len);
  }
}

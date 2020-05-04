#ifndef XV6_STRING_H
#define XV6_STRING_H

#include "base.h"

void memset(void *va, u8 pattern, int len);
int memcmp(const u8 *va, const char *string, int len);
u32 sum_bytes(u8 *va, int count);

#endif  // XV6_STRING_H

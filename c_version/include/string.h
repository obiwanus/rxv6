#ifndef XV6_STRING_H
#define XV6_STRING_H

#include "base.h"

void memset(void *va, u8 pattern, int len);
bool memcmp(void *va, char *string, int len);

#endif  // XV6_STRING_H

#ifndef XV6_BASE_H
#define XV6_BASE_H

typedef signed char i8;
typedef short int i16;
typedef int i32;
typedef long long i64;
typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned int uint;
typedef unsigned long long u64;

// Bool (can't include stdbool.h)
typedef u8 bool;
#define true 1
#define false 0

#define NULL 0

#define COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif

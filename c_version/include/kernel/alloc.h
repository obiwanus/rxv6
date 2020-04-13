#ifndef XV6_ALLOC_H
#define XV6_ALLOC_H

#include "base.h"
#include "kernel/mmu.h"
#include "kernel/spinlock.h"

typedef struct FreeMemoryList {
  struct FreeMemoryList *next;
} FreeMemoryList;

typedef struct {
  Spinlock lock;
  FreeMemoryList *free_list;
} KMemory;

void init_kernel_memory_range(void *vstart, void *vend);
void free_page(void *va);

#endif  // XV6_ALLOC_H

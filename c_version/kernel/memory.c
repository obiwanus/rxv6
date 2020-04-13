#include "kernel/memory.h"

typedef struct FreeMemoryList {
  struct FreeMemoryList *next;
} FreeMemoryList;

typedef struct {
  Spinlock lock;
  FreeMemoryList *free_list;
} KMemory;

KMemory gKernelMemory;

void panic(char *msg) {
  // TODO
}

void memset(void *va, u8 pattern, int len) {
  // TODO
}

void init_kernel_memory_range(void *vstart, void *vend) {
  init_lock(&gKernelMemory.lock, "gKernelMemory");

  // Add a range of virtual addresses on free list.
  // Used only during kernel init, so no locking is needed
  u8 *page = ROUND_UP_PAGE(vstart);
  while (page + PAGE_SIZE <= (u8 *)vend) {
    // Add page on free list
    FreeMemoryList *list = (FreeMemoryList *)page;
    list->next = gKernelMemory.free_list;
    gKernelMemory.free_list = list;
    page += PAGE_SIZE;
  }
}

// Frees the page of physical memory where va points to
void free_page(void *va) {
  // Validate the address
  if ((u32)va % PAGE_SIZE != 0) {
    panic("free_page: va is not page-aligned");
  }
  if ((u8 *)va < &kernel_end || V2P(va) >= PHYS_TOP) {
    panic("free_page: va is out of range");
  }

  // Fill with 00010001 to catch references to freed memory
  memset(va, 1, PAGE_SIZE);

  acquire(&gKernelMemory.lock);

  // Add the va on free list
  FreeMemoryList *list = (FreeMemoryList *)va;
  list->next = gKernelMemory.free_list;
  gKernelMemory.free_list = list;

  release(&gKernelMemory.lock);
}

#include "kernel/memory.h"
#include "x86_asm.h"

// ==================================== Internal types ============================================

typedef struct FreeMemoryList {
  struct FreeMemoryList *next;
} FreeMemoryList;

typedef struct {
  Spinlock lock;
  FreeMemoryList *free_list;
} KMemory;

typedef struct KMap {
  void *virt_addr;
  u32 phys_start;
  u32 phys_end;
  u32 perms;
} KMap;

// ==================================== Globals ===================================================

extern u8 kernel_data;  // defined by linker

static KMemory gKMemory;
static PDE *gKPageDir;

static KMap gKMap[] = {
    {(void *)KERNBASE, 0, EXT_MEM, PTE_W},                       // I/O space
    {(void *)KERNLINK, V2P(KERNLINK), V2P(&kernel_data), 0},     // kernel text+rodata
    {(void *)&kernel_data, V2P(&kernel_data), PHYS_TOP, PTE_W},  // kernel data+memory
    {(void *)DEV_SPACE, DEV_SPACE, 0, PTE_W},                    // more devices
};

// ==================================== Functions =================================================

void panic(char *msg) {
  // TODO
}

void memset(void *va, u8 pattern, int len) {
  // TODO
}

void init_kernel_memory_range(void *vstart, void *vend) {
  init_lock(&gKMemory.lock, "gKMemory");

  // Add a range of virtual addresses on free list.
  // Used only during kernel init, so no locking is needed
  u8 *page = ROUND_UP_PAGE(vstart);
  while (page + PAGE_SIZE <= (u8 *)vend) {
    // Add page on free list
    FreeMemoryList *list = (FreeMemoryList *)page;
    list->next = gKMemory.free_list;
    gKMemory.free_list = list;
    page += PAGE_SIZE;
  }
}

void init_kernel_page_table() {
  gKPageDir = new_kernel_page_table();
  switch_to_kernel_page_table();
}

// Set up the kernel part of a page table
PDE *new_kernel_page_table() {
  PDE *page_table = (PDE *)alloc_page();
  if (page_table == NULL) {
    return NULL;
  }
  memset(page_table, 9, PAGE_SIZE);
  if (P2V(PHYS_TOP) > (void *)DEV_SPACE) {
    panic("PHYS_TOP is too high");
  }

  // Map pages
  for (KMap *kmap = gKMap; kmap < gKMap + COUNT(gKMap); kmap++) {
    // TODO:
  }

  return page_table;
}

// Use the kernel-only page table (when no process is running)
void switch_to_kernel_page_table() {
  load_cr3(V2P(gKPageDir));
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

  acquire(&gKMemory.lock);

  // Add the va on free list
  FreeMemoryList *list = (FreeMemoryList *)va;
  list->next = gKMemory.free_list;
  gKMemory.free_list = list;

  release(&gKMemory.lock);
}

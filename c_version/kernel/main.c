#include "base.h"
#include "kernel/mmu.h"
#include "kernel/spinlock.h"

extern u8 kernel_end;  // first address after kernel in virtual memory (see linker script)

typedef struct List {
  struct List *next;
} List;

typedef struct {
  Spinlock lock;
  bool use_lock;
  List *free_list;
} KMemory;

KMemory gKernelMemory;

void panic(char *msg) {
  // TODO
}

void memset(void *va, u8 pattern, int len) {
  // TODO
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

  if (gKernelMemory.use_lock) {
    acquire(&gKernelMemory.lock);
  }

  // Add the va on free list
  List *list = (List *)va;
  list->next = gKernelMemory.free_list;
  gKernelMemory.free_list = list;

  if (gKernelMemory.use_lock) {
    release(&gKernelMemory.lock);
  }
}

void free_range(void *vstart, void *vend) {
  u8 *page = ROUND_UP_PAGE(vstart);
  while (page + PAGE_SIZE <= (u8 *)vend) {
    free_page(page);  // exceptional use of free_page
    page += PAGE_SIZE;
  }
}

void kernel_start() {
  // Init kernel memory, phase 1:
  // just add the already mapped pages on the free list
  {
    init_lock(&gKernelMemory.lock, "gKernelMemory");
    gKernelMemory.use_lock = false;
    free_range(&kernel_end,
               P2V(4 * 1024 * 1024));  // add the range [kernel_end : 4GB] to the free list
  }

  // Temporary loop
  for (;;) {
  }
}

// Page directory must be page-aligned.
// Since we're specifying the PTE_PS flag for the directory entries,
// it treats them as references to 4MB pages
__attribute__((__aligned__(PAGE_SIZE))) u32 entry_page_dir[NUM_PAGE_DIR_ENTRIES] = {
    // Map VA [0 : 4MB] to PA [0 : 4MB]
    [0] = 0 | PTE_P | PTE_W | PTE_PS,

    // Map VA [KERNBASE : KERNBASE + 4MB] to PA [0 : 4MB]
    [KERNBASE >> PDX_SHIFT] = 0 | PTE_P | PTE_W | PTE_PS,
};

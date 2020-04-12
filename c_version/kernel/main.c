#include "base.h"
#include "kernel/mmu.h"
#include "kernel/spinlock.h"

extern u8 *end;  // first address after kernel in physical memory (see linker script)

// TODO(Ivan): rename?
typedef struct Run {
  struct Run *next;
} Run;

typedef struct {
  Spinlock lock;
  bool use_lock;
  Run *free_list;
} KMemory;

KMemory gKernelMemory;

void free_range(void *start, void *end) {
  // TODO
}

void kernel_start() {
  // Init kernel memory, phase 1:
  // just add the already mapped pages on the free list
  {
    init_lock(&gKernelMemory.lock, "gKernelMemory");
    gKernelMemory.use_lock = false;
    free_range(end, P2V(4 * 1024 * 1024));  // add the range [end : 4GB] to the free list
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

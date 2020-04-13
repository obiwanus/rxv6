#include "kernel/memory.h"

void kernel_start() {
  // Add the already mapped pages on the free list
  init_kernel_memory_range(&kernel_end, P2V(4 * 1024 * 1024));  // [kernel_end : 4GB]

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

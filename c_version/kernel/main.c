#include "base.h"
#include "mmu.h"

void kernel_main() {
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

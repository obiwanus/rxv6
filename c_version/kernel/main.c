#include "base.h"
#include "mmu.h"

int main() {
  volatile int b = 0;
  for (int i = 0; i < 100500; ++i) {
    b += i;
  }

  return 123;
}

// TODO(Ivan): why are we aligning this?
__attribute__((__aligned__(PAGE_SIZE))) u32 entry_page_dir[NUM_PAGE_DIR_ENTRIES] = {
    // Map VA [0 : 4MB] to PA [0 : 4MB]
    [0] = 0 | PTE_P | PTE_W | PTE_PS,

    // Map VA [KERNBASE : KERNBASE + 4MB] to PA [0 : 4MB]
    [KERNBASE >> PDX_SHIFT] = 0 | PTE_P | PTE_W | PTE_PS,
};

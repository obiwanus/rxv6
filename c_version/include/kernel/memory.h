#ifndef XV6_MEMORY_H
#define XV6_MEMORY_H

#include "base.h"
#include "kernel/spinlock.h"

// ==================================== Types =====================================================

typedef u32 PDE;  // Page directory entry
typedef u32 PTE;  // Page table entry

// ==================================== Data ======================================================

extern u8 kernel_end;  // first address after kernel in virtual memory (see linker script)

#define EXT_MEM 0x100000               // start of extended memory
#define PHYS_TOP 0x0E000000            // physical memory ends here (224 MB)
#define DEV_SPACE 0xFE000000           // devices at high addresses
#define KERNBASE 0x80000000            // first kernel virtual address
#define KERNLINK (KERNBASE + EXT_MEM)  // address where the kernel is linked

#define V2P(a) ((u32)(a)-KERNBASE)
#define P2V(a) ((void *)(((u8 *)(a)) + KERNBASE))

#define PAGE_SIZE 4096  // 0x1000
#define NUM_PAGE_DIR_ENTRIES 1024

#define PTX_SHIFT 12  // offset of PTX in a linear address
#define PDX_SHIFT 22  // offset of PDX in a linear address

#define PAGE_DIR_INDEX(va) (((u32)(va) >> PDX_SHIFT) & 0x3FF)    // the first 10 bits of va
#define PAGE_TABLE_INDEX(va) (((u32)(va) >> PTX_SHIFT) & 0x3FF)  // the second 10 bits of va

// Page table/directory entry flags
#define PTE_P 0x0001   // Present
#define PTE_W 0x0002   // Writeable
#define PTE_U 0x0004   // User
#define PTE_PS 0x0080  // Page Size

// Parsing the page dir/table entries
#define PTE_ADDR(pte) ((u32)(pte) & ~0xFFF)
#define PTE_FLAGS(pte) ((u32)(pte)&0xFFF)

// E.g. 0001 0000 0000 0000 = page size, then
//      0000 1111 1111 1111 = (page size - 1).
// We can't just do ROUND_UP_PAGE(addr) as (ROUND_DOWN_PAGE(addr) + PAGE_SIZE) because
// if addr is currently on the page boundary we don't want it to change
#define ROUND_DOWN_PAGE(a) (u8 *)((u32)(a) & ~(PAGE_SIZE - 1))
#define ROUND_UP_PAGE(a) (u8 *)((u32)(a + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1))

// ==================================== Functions =================================================

void init_kernel_memory_range(void *vstart, void *vend);
void init_global_kernel_page_dir();

u8 *alloc_page();
void free_page(void *va);

#endif  // XV6_MEMORY_H

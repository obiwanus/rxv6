#ifndef XV6_MMU_H
#define XV6_MMU_H

#define KERNBASE 0x80000000

#define V2P(a) ((u32)(a)-KERNBASE)
#define P2V(a) ((void *)(((u8 *)(a)) + KERNBASE))

#define PAGE_SIZE 4096
#define NUM_PAGE_DIR_ENTRIES 1024

#define PTX_SHIFT 12  // offset of PTX in a linear address
#define PDX_SHIFT 22  // offset of PDX in a linear address

// Page table/directory entry flags
#define PTE_P 0x0001   // Present
#define PTE_W 0x0002   // Writeable
#define PTE_U 0x0004   // User
#define PTE_PS 0x0080  // Page Size

#endif  // XV6_MMU_H

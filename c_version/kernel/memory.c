#include "string.h"
#include "x86_asm.h"

#include "kernel/console.h"
#include "kernel/memory.h"

// ==================================== Internal types ============================================

typedef struct FreePage {
  struct FreePage *next;
} FreePage;

typedef struct KMemory {
  Spinlock lock;
  FreePage *free_list;
  bool use_lock;
} KMemory;

typedef struct KMap {
  u32 virt_addr;
  u32 phys_start;
  u32 phys_end;
  u32 perms;
} KMap;

// ==================================== Globals ===================================================

extern u8 kernel_data;  // defined by linker

static KMemory gKMemory;
static PDE *gKPageDir;

static KMap gKMap[] = {
    // {virt_addr, phys_addr, size_bytes, perm_flags}
    {KERNBASE, 0, EXT_MEM, PTE_W},                            // I/O space
    {KERNLINK, V2P(KERNLINK), V2P(&kernel_data), 0},          // kernel text+rodata
    {(u32)&kernel_data, V2P(&kernel_data), PHYS_TOP, PTE_W},  // kernel data+memory
    {DEV_SPACE, DEV_SPACE, 0, PTE_W},                         // more devices
};

// ==================================== Functions =================================================

void
free_page_dir(PDE *page_dir)
{
  // TODO
}

// Uses the page directory to get the page table entry for a virtual address
static PTE *
get_pte_for_va(PDE *page_dir, const void *va)
{
  PDE *pde = page_dir + PAGE_DIR_INDEX(va);
  if (!(*pde & PTE_P)) {
    return NULL;  // mapping doesn't exist
  }
  PTE *page_table = (PTE *)P2V(PTE_ADDR(*pde));
  return page_table + PAGE_TABLE_INDEX(va);
}

// Maps all the pages from [va: va+size] to [pa: pa+size].
// va and size might not be page-aligned
static bool
map_range(PDE *page_dir, u32 va, u32 pa, u32 size, u32 perms)
{
  // Get the page-aligned virtual address range
  u8 *start = ROUND_DOWN_PAGE(va);
  u8 *end = ROUND_UP_PAGE(va + size);

  for (u8 *addr = start; addr < end; addr += PAGE_SIZE, pa += PAGE_SIZE) {
    // Allocate the page table for va if not present
    {
      PDE *pde = page_dir + PAGE_DIR_INDEX(addr);
      if (!(*pde & PTE_P)) {
        u8 *page_table = alloc_page();
        if (page_table == NULL) {
          return false;
        }
        memset(page_table, 0, PAGE_SIZE);  // make sure it's a fresh page table
        // The permissions on the pde don't restrict anything.
        // They will be more fine-grained at the page table entry level.
        *pde = V2P(page_table) | PTE_W | PTE_U | PTE_P;
      }
    }

    PTE *pte = get_pte_for_va(page_dir, addr);
    if (pte == NULL) {
      return false;
    }
    if (*pte & PTE_P) {
      panic("Trying to remap an existing PTE");
    }
    *pte = pa | perms | PTE_P;
  }

  return true;
}

void
init_kernel_memory_range(void *vstart, void *vend)
{
  init_lock(&gKMemory.lock, "gKMemory");

  // Add a range of virtual addresses on free list.
  for (u8 *page = ROUND_UP_PAGE(vstart); page + PAGE_SIZE <= (u8 *)vend; page += PAGE_SIZE) {
    free_page(page);
  }
}

// Set up the kernel part of a page table
PDE *
new_page_dir_with_kernel_mappings()
{
  PDE *page_dir = (PDE *)alloc_page();
  if (page_dir == NULL) {
    return NULL;
  }
  memset(page_dir, 0, PAGE_SIZE);

  if (P2V(PHYS_TOP) > (void *)DEV_SPACE) {  // TODO: maybe we can check it elsewhere?
    panic("PHYS_TOP is too high");
  }

  // Setup the kernel mappings
  for (KMap *kmap = gKMap; kmap < gKMap + COUNT(gKMap); kmap++) {
    bool mapped = map_range(page_dir, kmap->virt_addr, kmap->phys_start,
                            kmap->phys_end - kmap->phys_start, kmap->perms);
    if (!mapped) {
      free_page_dir(page_dir);
      return NULL;
    }
  }

  return page_dir;
}

// Use the kernel-only page table (when no process is running)
void
switch_to_kernel_page_dir()
{
  load_cr3(V2P(gKPageDir));
}

void
init_global_kernel_page_dir()
{
  gKPageDir = new_page_dir_with_kernel_mappings();
}

// Frees the page of physical memory where va points to
void
free_page(void *va)
{
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
  FreePage *list = (FreePage *)va;
  list->next = gKMemory.free_list;
  gKMemory.free_list = list;

  release(&gKMemory.lock);
}

// Allocates a 4Kb page of physical memory.
// Returns a virtual address to the allocated memory
// Returns NULL if can't allocate anything.
u8 *
alloc_page()
{
  acquire(&gKMemory.lock);
  FreePage *list = gKMemory.free_list;
  if (list != NULL) {
    gKMemory.free_list = list->next;
  }
  release(&gKMemory.lock);
  return (u8 *)list;
}

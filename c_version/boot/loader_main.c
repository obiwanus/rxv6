#include "base.h"
#include "elf.h"
#include "x86_asm.h"

#define SECTOR_SIZE 512

void wait_for_disk_ready() {
  while ((in_u8(0x1F7) & 0xC0) != 0x40)
    ;
}

void read_sector(void *dst, u32 offset) {
  wait_for_disk_ready();

  // Specify the number of sectors to read
  out_u8(0x1F2, 1);  // count = 1

  // Specify the address of the logical block
  out_u8(0x1F3, (u8)offset);                 // LBA bits 0 to 7
  out_u8(0x1F4, (u8)(offset >> 8));          // LBA bits 8 to 15
  out_u8(0x1F5, (u8)(offset >> 16));         // LBA bits 16 to 23
  out_u8(0x1F6, (u8)(offset >> 24) | 0xE0);  // LBA bits 24 to 27

  // Issue command to read sectors
  out_u8(0x1F7, 0x20);  // 0x20 - read with retry

  // Read data
  wait_for_disk_ready();
  in_u32_array(0x1F0, dst, SECTOR_SIZE);
}

// Reads 'count' bytes off disk into 'dst', starting from sector 1.
// Makes sure the first byte at 'offset_in_bytes' will end up
// exactly at 'dst'.
// WARNING: may overwrite bytes located before 'dst' in order to achieve it.
void read_bytes_off_disk(u8 *dst, uint count, uint offset_in_bytes) {
  u8 *end_address = dst + count;

  // Rewind the starting point if necessary (see comment above)
  dst -= offset_in_bytes % SECTOR_SIZE;

  // We start from sector 1 - that's where the kernel is.
  // Sector 0 is reserved.
  uint offset_in_sectors = (offset_in_bytes / SECTOR_SIZE) + 1;

  // Read however many sectors is needed (will likely read more than 'count' bytes in total)
  while (dst < end_address) {
    read_sector(dst, offset_in_sectors);
    dst += SECTOR_SIZE;
    offset_in_sectors++;
  }
}

typedef void (*EntryFunc)(void);

int bootmain() {
  // Load some bytes off disk into scratch space
  ELF32_Header *elf_header = (ELF32_Header *)0x10000;
  read_bytes_off_disk((u8 *)elf_header, 4096, 0);

  // Is this probably an ELF executable?
  if (elf_header->magic != ELF_MAGIC) {
    return 1;  // back to loader.asm
  }

  // Load all program segments
  ELF32_ProgramHeader *ph     = (ELF32_ProgramHeader *)((u8 *)elf_header + elf_header->ph_offset);
  ELF32_ProgramHeader *end_ph = ph + elf_header->ph_entry_count;
  while (ph < end_ph) {
    u8 *program_section = (u8 *)ph->phys_addr;
    read_bytes_off_disk(program_section, ph->file_size, ph->offset);
    if (ph->mem_size > ph->file_size) {
      stosb(program_section + ph->file_size, 0, ph->mem_size - ph->file_size);
    }
    ph++;  // next entry
  }

  EntryFunc entry = (EntryFunc)elf_header->entry;
  entry();  // will not return

  return 1;  // but who knows
}

// ///////////////////////////////////////////////////////////////////////////////////

// // Boot loader.
// //
// // Part of the boot block, along with bootasm.S, which calls bootmain().
// // bootasm.S has put the processor into protected 32-bit mode.
// // bootmain() loads an ELF kernel image from the disk starting at
// // sector 1 and then jumps to the kernel entry routine.

// typedef unsigned int uint;
// typedef unsigned short ushort;
// typedef unsigned char uchar;
// typedef uint pde_t;

// // Format of an ELF executable file

// #define ELF_MAGIC 0x464C457FU  // "\x7FELF" in little endian

// // File header
// struct elfhdr {
//   uint magic;  // must equal ELF_MAGIC
//   uchar elf[12];
//   ushort type;
//   ushort machine;
//   uint version;
//   uint entry;
//   uint phoff;
//   uint shoff;
//   uint flags;
//   ushort ehsize;
//   ushort phentsize;
//   ushort phnum;
//   ushort shentsize;
//   ushort shnum;
//   ushort shstrndx;
// };

// // Program section header
// struct proghdr {
//   uint type;
//   uint off;
//   uint vaddr;
//   uint paddr;
//   uint filesz;
//   uint memsz;
//   uint flags;
//   uint align;
// };

// // Values for Proghdr type
// #define ELF_PROG_LOAD 1

// // Flag bits for Proghdr flags
// #define ELF_PROG_FLAG_EXEC 1
// #define ELF_PROG_FLAG_WRITE 2
// #define ELF_PROG_FLAG_READ 4

// // Routines to let C code use special x86 instructions.

// static inline uchar inb(ushort port) {
//   uchar data;

//   asm volatile("in %1,%0" : "=a"(data) : "d"(port));
//   return data;
// }

// static inline void insl(int port, void *addr, int cnt) {
//   asm volatile("cld; rep insl"
//                : "=D"(addr), "=c"(cnt)
//                : "d"(port), "0"(addr), "1"(cnt)
//                : "memory", "cc");
// }

// static inline void outb(ushort port, uchar data) {
//   asm volatile("out %0,%1" : : "a"(data), "d"(port));
// }

// static inline void stosb(void *addr, int data, int cnt) {
//   asm volatile("cld; rep stosb"
//                : "=D"(addr), "=c"(cnt)
//                : "0"(addr), "1"(cnt), "a"(data)
//                : "memory", "cc");
// }

// #define SECTSIZE 512

// void readseg(uchar *, uint, uint);

// void bootmain(void) {
//   struct elfhdr *elf;
//   struct proghdr *ph, *eph;
//   void (*entry)(void);
//   uchar *pa;

//   elf = (struct elfhdr *)0x10000;  // scratch space

//   // Read 1st page off disk
//   readseg((uchar *)elf, 4096, 0);

//   // Is this an ELF executable?
//   if (elf->magic != ELF_MAGIC)
//     return;  // let bootasm.S handle error

//   // Load each program segment (ignores ph flags).
//   ph  = (struct proghdr *)((uchar *)elf + elf->phoff);
//   eph = ph + elf->phnum;
//   for (; ph < eph; ph++) {
//     pa = (uchar *)ph->paddr;
//     readseg(pa, ph->filesz, ph->off);
//     if (ph->memsz > ph->filesz)
//       stosb(pa + ph->filesz, 0, ph->memsz - ph->filesz);
//   }

//   // Call the entry point from the ELF header.
//   // Does not return!
//   entry = (void (*)(void))(elf->entry);
//   entry();
// }

// void waitdisk(void) {
//   // Wait for disk ready.
//   while ((inb(0x1F7) & 0xC0) != 0x40)
//     ;
// }

// // Read a single sector at offset into dst.
// void readsect(void *dst, uint offset) {
//   // Issue command.
//   waitdisk();
//   outb(0x1F2, 1);  // count = 1
//   outb(0x1F3, offset);
//   outb(0x1F4, offset >> 8);
//   outb(0x1F5, offset >> 16);
//   outb(0x1F6, (offset >> 24) | 0xE0);
//   outb(0x1F7, 0x20);  // cmd 0x20 - read sectors

//   // Read data.
//   waitdisk();
//   insl(0x1F0, dst, SECTSIZE / 4);
// }

// // Read 'count' bytes at 'offset' from kernel into physical address 'pa'.
// // Might copy more than asked.
// void readseg(uchar *pa, uint count, uint offset) {
//   uchar *epa;

//   epa = pa + count;

//   // Round down to sector boundary.
//   pa -= offset % SECTSIZE;

//   // Translate from bytes to sectors; kernel starts at sector 1.
//   offset = (offset / SECTSIZE) + 1;

//   // If this is too slow, we could read lots of sectors at a time.
//   // We'd write more to memory than asked, but it doesn't matter --
//   // we load in increasing order.
//   for (; pa < epa; pa += SECTSIZE, offset++) readsect(pa, offset);
// }

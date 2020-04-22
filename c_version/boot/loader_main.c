#include "base.h"
#include "elf.h"
#include "x86_asm.h"

#define SECTOR_SIZE 512

void
wait_for_disk_ready() {
  while ((in_u8(0x1F7) & 0xC0) != 0x40)
    ;
}

void
read_sector(void *dst, u32 offset) {
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
  in_u32_array(0x1F0, dst, SECTOR_SIZE / 4);  // reads SECTOR_SIZE bytes
}

// Reads 'count' bytes off disk into 'dst', starting from sector 1.
// Makes sure the first byte at 'offset_in_bytes' will end up
// exactly at 'dst'.
// WARNING: may overwrite bytes located before 'dst' in order to achieve it.
void
read_bytes_off_disk(u8 *dst, uint count, uint offset_in_bytes) {
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

int
bootmain() {
  // Load some bytes off disk into scratch space
  ELF32_Header *elf_header = (ELF32_Header *)0x10000;
  read_bytes_off_disk((u8 *)elf_header, 4096, 0);

  // Is this probably an ELF executable?
  if (elf_header->magic != ELF_MAGIC) {
    return 1;  // back to loader.asm
  }

  // Load all program segments
  ELF32_ProgramHeader *ph = (ELF32_ProgramHeader *)((u8 *)elf_header + elf_header->ph_offset);
  ELF32_ProgramHeader *end_ph = ph + elf_header->ph_entry_count;
  while (ph < end_ph) {
    u8 *program_section = (u8 *)ph->phys_addr;
    read_bytes_off_disk(program_section, ph->file_size, ph->offset);
    if (ph->mem_size > ph->file_size) {
      store_u8s(program_section + ph->file_size, 0, ph->mem_size - ph->file_size);
    }
    ph++;  // next entry
  }

  EntryFunc entry = (EntryFunc)elf_header->entry;
  entry();  // will not return

  return 1;  // but who knows
}

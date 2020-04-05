#include "base.h"
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

int bootmain() {
  return 1;
}

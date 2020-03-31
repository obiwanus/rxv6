#include "base.h"
#include "x86_asm.h"

#define DISK_STATUS_READY 0x40
#define DISK_CMD_READ_WITH_RETRY 0x20
#define DISK_PORT_COMMAND 0x1F7
#define DISK_PORT_SECTOR_COUNT 0x1F2
#define DISK_PORT_SECTOR_INDEX 0x1F3
#define DISK_PORT_CYLINDER_LO 0x1F4
#define DISK_PORT_CYLINDER_HI 0x1F5
#define DISK_PORT_DRIVE_HEAD 0x1F6

void wait_for_disk_ready() {
  while ((in_u8(0x1F7) & 0xC0) != DISK_STATUS_READY)
    ;
}

void read_sector(void *dst, u32 offset) {
  // Issue command to read sector
  out_u8()
}

int bootmain() {
  int a = 1;
  int b = addone(a);

  return b;
}

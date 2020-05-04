// Multiprocessor support
// See the MP specification 1.4

#include "string.h"

#include "kernel/console.h"
#include "kernel/memory.h"
#include "kernel/mp.h"

// Search for the floating pointer struct at [addr : addr + len bytes]
static MP_FPStruct *
mp_search(u32 addr, int len)
{
  // for (u8 *p = P2V(addr); p < P2V(addr) + len; p += sizeof(MP_FPStruct)) {
  //   // if (memcmp(p, "_MP_", 4))
  // }
  return NULL;  // not found
}

// Search for the floating pointer structure in the locations defined in the spec
static MP_FPStruct *
mp_fp_struct_search()
{
  u8 *BDA = (u8 *)P2V(0x400);

  // Check the first KB of the extended BIOS data area (EBDA)
  {
    u32 addr = (BDA[0x0F] << 8) | (BDA[0x0E] << 4);
    log("Searching for an MP struct in range [%p : %p]...", addr, addr + 1024);
    MP_FPStruct *result = mp_search(addr, 1024);
    if (result)
      return result;

    log("Not found");
  }

  // Check the last KB of system base memory
  {
    u32 addr = ((BDA[0x14] << 8) | BDA[0x13]) * 1024;
    log("Searching for an MP struct in range [%p : %p]", addr, addr + 1024);
    MP_FPStruct *result = mp_search(addr - 1024, 1024);
    if (result)
      return result;
    log("Not found");
  }

  // Check the BIOS ROM between 0xE0000 and 0xFFFFF
  return mp_search(0xE0000, 0x10000);
}

// Search for an MP configuration table.
// For simplicity we don't accept any default configurations
// and require the table to be present.
static bool
find_mp_config(MP_FPStruct **p_mp_struct, MP_ConfigTable **p_mp_table)
{
  MP_FPStruct *mp_struct = mp_fp_struct_search();
  if (mp_struct) {
    return true;
  }
  return false;
}

void
mp_init()
{
  MP_FPStruct *mp_fp_struct;
  MP_ConfigTable *mp_config_table;

  // Try to find an MP config table
  bool found = find_mp_config(&mp_fp_struct, &mp_config_table);
  if (!found)
    panic("Couldn't find a valid MP config table. Probably not an SMP system.");
}

// Multiprocessor support
// See the MP specification 1.4

#include "kernel/mp.h"

// Search for the floating pointer structure in the locations defined in the spec
static MP_FloatingPointerStruct *
mp_fp_struct_search()
{
  MP_FloatingPointerStruct *result;

  u8 *BDA = (u8 *)P2V(0x400);

  // Check the first KB of the extended BIOS data area (EBDA)
  {
    u32 addr = (BDA[0x0F] << 8) | (BDA[0x0E] << 4);
    result = mp_search(addr, 1024);
    if (result) {
      return true;
    }
  }

  // Check the last KB of system base memory
  {
    u32 addr = ((BDA[0x14] << 8) | BDA[0x13]) * 1024;
    result = mp_search(addr - 1024, 1024);
    if (result) {
      return result;
    }
  }

  // !!!!!!!!!!!!!!!!!!!!!!
}

// Search for an MP configuration table.
// For simplicity we don't accept any default configurations
// and require the table to be present.
static bool
find_mp_config(MP_FloatingPointerStruct **p_mp_struct, MP_ConfigTable *p_mp_table)
{
  MP_FloatingPointerStruct *mp_struct = mp_fp_struct_search();
}

void
mp_init()
{
  MP_FloatingPointerStruct *mp_floating_pointer_struct;
  MP_ConfigTable *mp_config_table;

  // Try to find an MP config table
  bool found = find_mp_config(&mp_floating_pointer_struct, &mp_config_table);
  if (!found) {
    panic("Couldn't find a valid MP config table. Probably not an SMP system.");
  }
}

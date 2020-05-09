// Multiprocessor support
// See the MP specification 1.4

#include "string.h"

#include "kernel/console.h"
#include "kernel/memory.h"
#include "kernel/mp.h"

// Search for the floating pointer struct at [addr : addr + len bytes]
static MP_FPStruct *
mp_search(u32 phys_addr, int len)
{
  u8 *addr = P2V(phys_addr);
  LOG_INFO("Searching for an MP struct in range [%p : %p]", addr, addr + len);
  for (u8 *p = addr; p < addr + len; p += sizeof(MP_FPStruct)) {
    if (memcmp(p, "_MP_", 4) == 0) {
      if (sum_bytes(p, sizeof(MP_FPStruct)) == 0) {
        LOG_INFO("Found at address %p", p);
        return (MP_FPStruct *)p;
      } else {
        LOG_ERROR("MP struct with a wrong checksum at %p", p);
      }
    }
  }
  LOG_INFO("mp_search: Not found");
  return NULL;  // not found
}

// Search for the floating pointer structure in the locations defined in the spec
static MP_FPStruct *
mp_fp_struct_search()
{
  u8 *BDA = (u8 *)P2V(0x400);

  // Check the first KB of the extended BIOS data area (EBDA)
  {
    u32 addr = ((BDA[0x0F] << 8) | BDA[0x0E]) << 4;
    MP_FPStruct *result = mp_search(addr, 1024);
    if (result)
      return result;
  }

  // Check the last KB of system base memory
  {
    u32 addr = ((BDA[0x14] << 8) | BDA[0x13]) * 1024;
    MP_FPStruct *result = mp_search(addr - 1024, 1024);
    if (result)
      return result;
  }

  // Check the BIOS ROM between 0xE0000 and 0xFFFFF
  return mp_search(0xE0000, 0x20000);
}

// Search for an MP configuration table.
// For simplicity we don't accept any default configurations
// and require the table to be present.
static bool
find_mp_config(MP_FPStruct **p_mp_struct, MP_ConfigTable **p_mp_table)
{
  // Find the FP struct
  MP_FPStruct *mp_struct = mp_fp_struct_search();
  if (mp_struct == 0) {
    LOG_ERROR("Unable to find the floating pointer struct");
    return false;
  }
  if (mp_struct->config_table_phys_addr == 0) {
    LOG_ERROR("We do not support default MP configurations");
    return false;
  }

  // Get the configuration table
  MP_ConfigTable *config = (MP_ConfigTable *)P2V(mp_struct->config_table_phys_addr);
  if (memcmp(config->signature, "PCMP", 4) != 0) {
    LOG_ERROR("Invalid MP config table signature");
    return false;
  }
  if (config->version != 1 && config->version != 4) {
    LOG_ERROR("Unsupported config version: %d", config->version);
    return false;
  }
  if (sum_bytes((u8 *)config, config->length) != 0) {
    LOG_ERROR("Invalid MP config table checksum");
    return false;
  }

  // Found everything
  *p_mp_struct = mp_struct;
  *p_mp_table = config;
  return true;
}

void
mp_init()
{
  MP_FPStruct *mp_fp_struct;
  MP_ConfigTable *mp_config_table;

  // Try to find an MP config table
  bool found = find_mp_config(&mp_fp_struct, &mp_config_table);
  if (!found)
    PANIC("Couldn't find a valid MP config table. Probably not an SMP system.");
}

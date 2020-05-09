#ifndef XV6_MP_H
#define XV6_MP_H

#include "base.h"

// ==================================== Types =====================================================

typedef struct MP_FPStruct {
  u8 signature[4];               // must be equal to "_MP_"
  void *config_table_phys_addr;  // contains the address of MP_ConfigTable if present
  u8 length;
  u8 spec_rev;
  u8 checksum;
  u8 sys_config_type;
  u8 imcr_present;
  u8 reserved[3];
} MP_FPStruct;

typedef struct MP_ConfigTable {
  u8 signature[4];  // must be equal to "PCMP"
  u16 length;
  u8 version;
  u8 checksum;
  u8 oem_id[8];
  u8 product_id[12];
  u32 *oem_table;
  u16 oem_table_size;
  u16 entry_count;
  u32 *lapic_addr;
  u16 ext_table_length;
  u8 ext_table_checksum;
  u8 reserved;
} MP_ConfigTable;

// ==================================== Functions =================================================

void mp_init();

#endif  // XV6_MP_H

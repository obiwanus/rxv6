#ifndef XV6_MP_H
#define XV6_MP_H

#include "base.h"

// ==================================== Types =====================================================

// Floating pointer struct
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

// Config table header
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

typedef struct MP_ProcEntry {
  u8 type;            // entry type (0 for proc)
  u8 apic_id;         // local APIC id
  u8 version;         // local APIC version
  u8 flags;           // CPU flags
  u8 signature[4];    // CPU signature
  u32 feature_flags;  // feature flags from CPUID instruction
  u8 reserved[8];
} MP_ProcEntry;

typedef struct MP_IoApicEntry {
  u8 type;     // entry type (2 for io apic)
  u8 apic_id;  // IO APIC id
  u8 version;  // IO APIC version
  u8 flags;    // IO APIC flags
  u32 *addr;   // IO APIC address
} MP_IoApicEntry;

// Table entry types
#define MP_ENTRY_PROC 0x00    // One per CPU
#define MP_ENTRY_BUS 0x01     // One per bus
#define MP_ENTRY_IOAPIC 0x02  // One per IO APIC
#define MP_ENTRY_IOINTR 0x03  // One per bus interrupt source
#define MP_ENTRY_LINTR 0x04   // One per system interrupt source

// ==================================== Functions =================================================

void mp_init();

#endif  // XV6_MP_H

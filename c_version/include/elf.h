#ifndef XV6_ELF_H
#define XV6_ELF_H

#include "base.h"

#define ELF_MAGIC 0x464C457FU  // 0x7FELF in little endian

typedef struct ELF32_Header {
  u32 magic;  // must equal ELF_MAGIC
  u8 ident[12];
  u16 type;
  u16 machine;
  u32 version;
  u32 entry;
  u32 ph_offset;
  u32 sh_offset;
  u32 flags;
  u16 header_size;
  u16 ph_entry_size;
  u16 ph_entry_count;
  // ... (not complete)
} ELF32_Header;

typedef struct ELF32_ProgramHeader {
  u32 type;
  u32 offset;
  u32 virt_addr;
  u32 phys_addr;
  u32 file_size;
  u32 mem_size;
  u32 flags;
  u32 align;
} ELF32_ProgramHeader;

// Program header type
#define ELF_PH_TYPE_LOAD 1

// Program header flags
#define ELF_PH_FLAG_EXEC 1
#define ELF_PH_FLAG_WRITE 2
#define ELF_PH_FLAG_READ 4

#endif

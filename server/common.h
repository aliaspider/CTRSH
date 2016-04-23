#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <sys/cdefs.h>

typedef enum
{
   CTRSH_COMMAND_INVALID,
   CTRSH_COMMAND_EXIT,
   CTRSH_COMMAND_DISPLAY_IMAGE,
   CTRSH_COMMAND_DIRENT,

   CTRSH_COMMAND_ID_MAX
}ctrsh_command_id_t;


typedef struct __attribute((packed))
{
   uint32_t entry_size;
   uint32_t attributes;
   uint64_t size;
   uint8_t name [];
}ctrsh_dirent;

#endif // COMMON_H

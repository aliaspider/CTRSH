#ifndef SERVER_CMD_H
#define SERVER_CMD_H

#include <stdint.h>
#include <sys/cdefs.h>

typedef enum
{
   CTRSH_COMMAND_DIRENT,
   CTRSH_COMMAND_DISPLAY_IMAGE,
   CTRSH_COMMAND_EXIT,
   CTRSH_COMMAND_PUT,
}ctrsh_command_id_t;


typedef struct __attribute((packed))
{
   uint16_t entry_size;
   uint16_t mbslen;
   union
   {
      struct
      {
         uint8_t is_directory;
         uint8_t is_hidden;
         uint8_t is_archive;
         uint8_t is_read_only;
      };
      uint32_t attributes;
   };
   uint64_t file_size;
   unsigned char name [];
}ctrsh_dirent;

#endif // SERVER_CMD_H

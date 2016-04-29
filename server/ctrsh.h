#ifndef CTRSH_H
#define CTRSH_H

#include <3ds/types.h>
#include <3ds/services/fs.h>
#include "server_cmd.h"
#include "commands.h"

#define CTRSH_SERVER_SNDRCV_BUFFER_SIZE 0x8000
#define CTRSH_FILE_BUFFER_SIZE 0x8000


typedef struct
{
   Handle socket;
   Handle client;
   FS_Archive sdmc;
   ctrsh_command_t commands[CTRSH_COMMAND_ID_MAX];
}ctrsh_t;

extern ctrsh_t ctrsh;

#endif // CTRSH_H

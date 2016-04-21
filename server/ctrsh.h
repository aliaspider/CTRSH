#ifndef CTRSH_H
#define CTRSH_H

#include <3ds/types.h>
#include <3ds/services/fs.h>
#include "common.h"
#include "commands.h"

typedef struct
{
   Handle socket;
   Handle client;
   FS_Archive sdmc;
   ctrsh_command_t commands[CTRSH_COMMAND_ID_MAX];
}ctrsh_t;

extern ctrsh_t ctrsh;

#endif // CTRSH_H

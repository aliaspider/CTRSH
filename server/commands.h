#ifndef COMMANDS_H
#define COMMANDS_H

#include <3ds/types.h>
#include "server_cmd.h"
#include "ctr/ctr_net.h"

typedef void (*ctrsh_command_fn_t)();

typedef struct
{
   ctrsh_command_id_t id;
   const char* name;
   ctrsh_command_fn_t fn;
}ctrsh_command_t;



void ctrsh_wait_command(void);

#endif // COMMANDS_H

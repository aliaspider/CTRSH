
#include <stdio.h>

#include "ctr/ctr_debug.h"
#include "net/net_ctrl.h"

#include "common.h"
#include "commands.h"
#include "server_cmd.h"

void command_put_entry()
{
   printf("recieving file ...\n");
   DEBUG_ERROR(ctrsh_recv_to_file(0));
}

ctrsh_command_t command_put =
{
   CTRSH_COMMAND_PUT,
   "put",
   command_put_entry
};

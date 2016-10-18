#include <stdio.h>

#include "common.h"
#include "commands.h"
#include "serverctrl/server_ctrl.h"

void command_exit(char* const* options)
{
   int i;
   DEBUG_ERROR(send_command(CTRSH_COMMAND_EXIT));
   ctrsh.server.running = false;
}

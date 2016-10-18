#include <stdio.h>

#include "common.h"
#include "commands.h"
#include "serverctrl/server_ctrl.h"

void command_exit(char* const* options)
{
   int i;
   server_exit();
   ctrsh.server.running = false;
}

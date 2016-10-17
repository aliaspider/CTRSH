#include <stdio.h>

#include "common.h"
#include "commands.h"
#include "../server/server_cmd.h"

void command_exit(int sockfd, char* const* options)
{
   int i;
   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_EXIT));
   ctrsh.server_running = false;
}

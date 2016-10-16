#include <stdio.h>

#include "common.h"
#include "commands.h"
#include "../server/server_cmd.h"

void command_quit(int sockfd, int argc, char* const* argv)
{
   int i;
   printf("executing quit with arguments:\n");

   for (i = 0; i < argc; i++)
      printf("%s\n", argv[i]);

   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_EXIT));
   ctrsh.server_running = false;
   ctrsh.running = false;

}

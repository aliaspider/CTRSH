#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "commands.h"
#include "../server/server_cmd.h"

void command_put(int sockfd, char* const* options)
{
   int i;
   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_PUT));
   uint32_t filesize = 0x1000000;
   DEBUG_ERROR(write(sockfd, &filesize, 4));
   void* buffer =  malloc(filesize);
   DEBUG_ERROR(write(sockfd, buffer, filesize));
   free(buffer);
}

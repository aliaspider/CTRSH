#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "commands.h"
#include "serverctrl/server_ctrl.h"

void command_put(char* const* options)
{
   int i;
   DEBUG_ERROR(send_command(CTRSH_COMMAND_PUT));
   uint32_t filesize = 0x1000000;
   DEBUG_ERROR(send_data(&filesize, 4));
   void* buffer =  malloc(filesize);
   DEBUG_ERROR(send_data(buffer, filesize));
   free(buffer);
}

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "commands.h"
#include "serverctrl/server_ctrl.h"

void command_put(char* const* options)
{
   void* buffer =  malloc(0x1000000);
   server_put(buffer, 0x1000000);
   free(buffer);
}

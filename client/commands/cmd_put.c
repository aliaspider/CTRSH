#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "commands.h"
#include "serverctrl/server_ctrl.h"

static void command_put_entry(char* const* options)
{
   void* buffer =  malloc(0x1000000);
   server_put(buffer, 0x1000000);
   free(buffer);
}


command_t command_put = {
   "put",
   command_put_entry,
   NULL
};

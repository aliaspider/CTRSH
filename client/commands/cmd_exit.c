#include <stdio.h>

#include "common.h"
#include "commands.h"
#include "serverctrl/server_ctrl.h"

static void command_exit_entry(char* const* options)
{
   server_exit();
}


command_t command_exit = {
   "exit",
   command_exit_entry,
   NULL
};

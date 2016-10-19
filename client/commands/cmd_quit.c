#include <stdio.h>

#include "common.h"
#include "commands.h"
#include "serverctrl/server_ctrl.h"

static void command_quit_entry(char* const* options)
{
   server_exit();
   ctrsh.running = false;
}

command_t command_quit =
{
   "quit",
   command_quit_entry,
   NULL
};

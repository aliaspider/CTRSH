#include "common.h"
#include "commands.h"
#include "server_cmd.h"

void command_exit_entry()
{

}

ctrsh_command_t command_exit =
{
   CTRSH_COMMAND_EXIT,
   "exit",
   command_exit_entry
};

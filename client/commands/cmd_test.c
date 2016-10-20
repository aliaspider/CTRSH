#include <stdio.h>

#include "common.h"
#include "commands.h"
#include "serverctrl/server_ctrl.h"
#include "serverctrl/server_internal.h"

static void command_test_entry(char* const* options)
{
   DEBUG_ERROR(server_send_command(0x100));
}


command_t command_test = {
   "test",
   command_test_entry,
   NULL
};

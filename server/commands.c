
#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

#include "common.h"
#include "commands.h"
#include "server_cmd.h"

#include "ctr/ctr_net.h"
#include "ctr/ctr_debug.h"

#include "net/net_ctrl.h"

extern ctrsh_command_t command_dirent;
extern ctrsh_command_t command_display_image;
extern ctrsh_command_t command_exit;
extern ctrsh_command_t command_put;

ctrsh_command_t* ctrsh_commands [] =
{
   &command_dirent,
   &command_display_image,
   &command_exit,
   &command_put,
   NULL
};

void ctrsh_wait_command(void)
{
   u32 command_id = 0;

   while (aptMainLoop())
   {
      DEBUG_ERROR(recv_u32(&command_id));

      ctrsh_command_t** cmd = ctrsh_commands;

      while (*cmd)
      {
         if ((*cmd)->id == command_id)
         {
            profiler_start();
            (*cmd)->fn();
            profiler_stop();
            profiler_print((*cmd)->name);
            break;
         }

         cmd++;
      }

      if (!*cmd)
         printf("unknown command (%u).\n", (unsigned)command_id);


      if (command_id == CTRSH_COMMAND_EXIT)
         break;
   }
}

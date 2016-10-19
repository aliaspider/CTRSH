
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

void ctrsh_wait_command(void) // client, &client_addr
{
   u32 command_id = 0;

   while (aptMainLoop())
   {
      DEBUG_ERROR(ctrnet_recv(ctrsh.server.client, &command_id, 4, 0, &ctrsh.server.client_addr));

      ctrsh_command_t** cmd = ctrsh_commands;

      while(*cmd)
      {
         if((*cmd)->id == command_id)
         {
            u64 start_tick = svcGetSystemTick();
            (*cmd)->fn();
            u64 end_tick = svcGetSystemTick();
            printf("<%s> executed in %.3fms\n", (*cmd)->name, (end_tick - start_tick) / 268123.480);
            break;
         }
         cmd++;
      }
      if(!*cmd)
         printf("unknown command (%u).\n", (unsigned)command_id);


      if (command_id == CTRSH_COMMAND_EXIT)
         break;
   }
}

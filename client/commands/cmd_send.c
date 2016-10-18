#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "commands.h"
#include "../server/server_cmd.h"

void command_send(int sockfd, char* const* options)
{
   int i;
   FILE* rgui_fp;
   struct
   {
      void* buffer;
      size_t size;
   } rgui;

   DEBUG_ERROR(rgui_fp = fopen("./rgui.dat", "rb"));
   fseek(rgui_fp, 0, SEEK_END);
   rgui.size = ftell(rgui_fp);
   rgui.buffer = malloc(rgui.size);
   fseek(rgui_fp, 0, SEEK_SET);
   fread(rgui.buffer, 1, rgui.size, rgui_fp);
   fclose(rgui_fp);

   rl_printf_info("sending file\n");
   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_DISPLAY_IMAGE));
   DEBUG_ERROR(write(sockfd, &rgui.size, 4));
   DEBUG_ERROR(write(sockfd, rgui.buffer, rgui.size));
   free(rgui.buffer);

}

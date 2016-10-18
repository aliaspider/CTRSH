#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "commands.h"
#include "serverctrl/server_ctrl.h"

static void command_send_entry(char* const* options)
{
   int i;
   FILE* rgui_fp;
   void* buffer;
   size_t size;

   DEBUG_ERROR(rgui_fp = fopen("./rgui.dat", "rb"));
   fseek(rgui_fp, 0, SEEK_END);
   size = ftell(rgui_fp);
   buffer = malloc(size);
   fseek(rgui_fp, 0, SEEK_SET);
   fread(buffer, 1, size, rgui_fp);
   fclose(rgui_fp);

   server_display_image(buffer, size);

   free(buffer);

}


command_t command_send = {
   "send",
   command_send_entry,
   NULL
};

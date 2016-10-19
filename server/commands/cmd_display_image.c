#include <stdlib.h>
#include <3ds/gfx.h>

#include "common.h"
#include "commands.h"
#include "server_cmd.h"

#include "ctr/ctr_debug.h"
#include "net/net_ctrl.h"


void command_display_image_entry()
{
   void* file_buffer = NULL;
   ssize_t file_size = ctrsh_recv_to_buffer(&file_buffer);
   DEBUG_ERROR(file_size);

   if (file_buffer)
   {
      u16 fb_w, fb_h;
      u16* top_fb = (u16*)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &fb_w, &fb_h);

      int x, y;
      u16* rgui_buffer = (u16*)file_buffer;

      for (y = 0; y < 320; y++)
         for (x = 0; x < fb_w; x++)
            top_fb[x + ((y + 40) * fb_w)] = rgui_buffer[y + (fb_w - x - 1) * 320];

      free(file_buffer);
      gfxFlushBuffers();
   }
}

ctrsh_command_t command_display_image =
{
   CTRSH_COMMAND_DISPLAY_IMAGE,
   "display image",
   command_display_image_entry
};

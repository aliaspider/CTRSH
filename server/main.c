#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include "common.h"
#include "ctr/ctr_debug.h"
#include "ctr/ctr_net.h"
#include "commands.h"
#include "net/net_ctrl.h"
#include "utils/sdmc.h"

u32 __stacksize__ = 0x100000;

void wait_for_input(void)
{
   printf("\n\nPress Start.\n\n");
   fflush(stdout);

   while (aptMainLoop())
   {
      u32 kDown;

      hidScanInput();

      kDown = hidKeysDown();

      if (kDown & KEY_START)
         break;

      if (kDown & KEY_SELECT)
         exit(0);

      svcSleepThread(1000000);
   }
}

int main(int argc, char** argv)
{
   gfxInit(GSP_RGBA4_OES, GSP_RGB565_OES, false);
   gfxSet3D(false);
   gfxSetDoubleBuffering(GFX_TOP, false);
   consoleInit(GFX_BOTTOM, NULL);

   printf("CTRSH\n");
   sdmc_init();
   server_init();
   ctrsh_wait_command();
   server_exit();
   sdmc_exit();

   gfxExit();
   return 0;
}






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

u32 __stacksize__ = 0x100000;

ctrsh_t ctrsh;

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
   printf("Press Start to exit.\n");

   FSUSER_OpenArchive(&ctrsh.sdmc, ARCHIVE_SDMC, fsMakePath(PATH_ASCII,""));


//   u32 frames = 0;


   server_init();
   ctrsh_wait_command();
   server_deinit();

   FSUSER_CloseArchive(ctrsh.sdmc);

//   ndmuInit();
//   ndmuLeaveExclusiveState();
//   ndmuResumeScheduler();
//   ndmuResumeDaemons(0xF);
//   ndmuExit();

//   printf("\n");

//   while (aptMainLoop())
//   {
//      hidScanInput();
//      u32 kDown = hidKeysDown();

//      if (kDown & KEY_START)
//         break;

//      printf("frames : %u\r", (unsigned)frames++);
//      gfxFlushBuffers();
//      gspWaitForVBlank();
//      fflush(stdout);
//   }


   gfxExit();
   return 0;
}






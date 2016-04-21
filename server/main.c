#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <wchar.h>
#include "ctrsh.h"
#include "ctr_debug.h"
#include "ctr_net.h"
#include "commands.h"

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
   printf("Press Start to exit.\n");

   ctrsh.sdmc.id = ARCHIVE_SDMC;
   FSUSER_OpenArchive(&ctrsh.sdmc);

   Handle socket;
   Handle client;
   ctrnet_sockaddr_in_t host_addr = {0};
   ctrnet_sockaddr_in_t client_addr = {0};

   u32 frames = 0;

   DEBUG_ERROR(ctrnet_init(0x100000));

   host_addr.size = sizeof(host_addr);
   host_addr.family = AF_INET;
   host_addr.port = htons(5000);
   DEBUG_ERROR(ctrnet_gethostid(&host_addr.addr));
   DEBUG_ERROR(ctrnet_socket(&socket));

   u32 sockopt_val = 0xFFFF;
   ctrnet_setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &sockopt_val, 4);
   ctrnet_setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &sockopt_val, 4);

   DEBUG_ERROR(ctrnet_bind(socket, &host_addr));
   DEBUG_ERROR(ctrnet_listen(socket, 1));

   printf("IP %s\n", ctrnet_sa_to_cstr(&host_addr));

   do
   {
      Result ret = ctrnet_accept(socket, &client, &client_addr);
      DEBUG_ERROR(ret);
      if (!ret)
         break;
   }
   while (aptMainLoop());

   printf("Connection from %s\n", ctrnet_sa_to_cstr(&client_addr));

   ctrsh_wait_command(client, &client_addr);

   ctrnet_close(client);
   ctrnet_close(socket);
   ctrnet_exit();
   FSUSER_CloseArchive(&ctrsh.sdmc);


   printf("\n");

   while (aptMainLoop())
   {
      hidScanInput();
      u32 kDown = hidKeysDown();

      if (kDown & KEY_START)
         break;

      printf("frames : %u\r", (unsigned)frames++);
      gfxFlushBuffers();
      gspWaitForVBlank();
      fflush(stdout);
   }


   gfxExit();
   return 0;
}






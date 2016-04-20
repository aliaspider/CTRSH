#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "ctr_debug.h"
#include "ctr_net.h"

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

   Handle socket;
   Handle client;
   ctrnet_sockaddr_in_t host_addr = {0};
   ctrnet_sockaddr_in_t client_addr = {0};

   u32 frames = 0;
   u8* file_buffer = NULL;

   DEBUG_ERROR(ctrnet_init(0x100000));

   host_addr.size = sizeof(host_addr);
   host_addr.family = AF_INET;
   host_addr.port = htons(5000);
   DEBUG_ERROR(ctrnet_gethostid(&host_addr.addr));
   DEBUG_ERROR(ctrnet_socket(&socket));
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

   if(client_addr.addr)
   {
      int file_size = 0;
      while(!file_size)
         ctrnet_recv(client, &file_size, 4, 0, &client_addr);
      DEBUG_VAR(file_size);

      file_buffer = malloc(file_size);

      int recv_size = 0;
      while(recv_size < file_size)
      {
         u32 recvd;
         recvd = ctrnet_recv(client, file_buffer + recv_size, file_size - recv_size, 0, &client_addr);
         DEBUG_VAR(recvd);
         DEBUG_ERROR(recvd);
         if(recvd < 0)
            break;
         recv_size += recvd;

      }
      printf("recieved : %i bytes\n", recv_size);
   }
   ctrnet_close(client);
   ctrnet_close(socket);
   ctrnet_exit();

   if (file_buffer)
   {
      u16 fb_w, fb_h;
      u16* top_fb = (u16*)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &fb_w, &fb_h);

      int x, y;
      u16* rgui_buffer = (u16*)file_buffer;
      for (y = 0; y < 320; y++)
         for (x = 0; x < fb_w; x++)
            top_fb[x + ((y + 40) * fb_w)] = rgui_buffer[y + (fb_w - x - 1)* 320];

      free(file_buffer);
   }

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






#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <sys/_default_fcntl.h>
#include <netinet/in.h>
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
   ctrnet_sockaddr_t client_addr = {0};
   u32 host_id;
   u32 frames = 0;
   u8* file_buffer = NULL;

   DEBUG_ERROR(ctrnet_init(0x100000));
   DEBUG_ERROR(ctrnet_gethostid(&host_id));
   DEBUG_ERROR(ctrnet_socket(&socket));
   DEBUG_ERROR(ctrnet_bind(socket, host_id, 5000));
   DEBUG_ERROR(ctrnet_listen(socket, 1));

   printf("[x] IP %s:%d\n", inet_ntoa(*(struct in_addr*)&host_id), 5000);

   do
   {
      hidScanInput();
      u32 kDown = hidKeysDown();

      if (kDown & KEY_B)
      {
         printf("[!] Aborted\n");
         ctrnet_close(socket);
         break;
      }

      Result ret = ctrnet_accept(socket, &client, &client_addr);
      DEBUG_ERROR(ret);

      if (!ret)
         break;
   }
   while (aptMainLoop());

   printf("[x] Connection from %s:%d\n", inet_ntoa(*(struct in_addr*)&client_addr.ip),
          ntohs(client_addr.port));

   if(client_addr.ip)
   {
      u32 total = 0;

      u32 file_size = 0;
      while(!file_size)
         ctrnet_recv(client, &file_size, 4, 0, &client_addr);
      DEBUG_VAR(file_size);

      file_buffer = malloc(file_size);

      u32 recv_size = 0;
      while(recv_size < file_size)
      {
         u32 recvd;
         recvd = ctrnet_recv(client, file_buffer + recv_size, file_size - recv_size, 0, &client_addr);
         DEBUG_CERROR(recvd);
         if(recvd < 0)
            break;
         recv_size += recvd;

      }
      printf("recieved : %i bytes\n", recv_size);

//      const char* answer_str = "3DS says hi !!";
//      ctrnet_send(client, answer_str, strlen(answer_str) + 1, 0, &client_addr);
   }
   ctrnet_close(client);
   ctrnet_close(socket);
   ctrnet_exit();

   if (file_buffer)
   {
      u16 fb_w, fb_h;
      u16* top_fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &fb_w, &fb_h);

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






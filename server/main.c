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

   u32 frames = 0;

   u32* SOC_buffer = (u32*)memalign(0x1000, 0x100000);
   DEBUG_ERROR(socInit(SOC_buffer, 0x100000));


   s32 sockfd;
   s32 clientfd;
   struct sockaddr_in sa;
   struct sockaddr_in client_addr;
   socklen_t addrlen = sizeof(client_addr);
   s32 sflags = 0;

   DEBUG_CERROR(sockfd = socket(AF_INET, SOCK_STREAM, 0));

   memset(&sa, 0x00, sizeof(sa));
   sa.sin_family = AF_INET;
   sa.sin_port = htons(5000);
   sa.sin_addr.s_addr = gethostid();

   DEBUG_CERROR(bind(sockfd, (struct sockaddr*)&sa, sizeof(sa)));
   DEBUG_CERROR(listen(sockfd, 1));
   printf("[x] IP %s:%d\n", inet_ntoa(sa.sin_addr), 5000);

   do
   {
      hidScanInput();
      u32 kDown = hidKeysDown();

      if (kDown & KEY_B)
      {
         printf("[!] Aborted\n");
         close(sockfd);
         break;
      }

      clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
      svcSleepThread(100000000);

      if (clientfd > 0)
         break;
   }
   while (aptMainLoop());

   printf("[x] Connection from %s:%d\n", inet_ntoa(client_addr.sin_addr),
          ntohs(client_addr.sin_port));

   u32 total = 0;
   char buffer[256] = {0};
   char* write_ptr = buffer;

   u32 file_size = 0;
   while(!file_size)
      recv(clientfd, &file_size, 4, 0);
   DEBUG_VAR(file_size);

   u8* file_buffer = malloc(file_size);

   u32 recv_size = 0;
   while(recv_size < file_size)
   {
      u32 recvd;
      recvd = recv(clientfd, file_buffer + recv_size, file_size - recv_size, 0);
      DEBUG_VAR(recvd);
      DEBUG_CERROR(recvd);
      if(recvd < 0)
         break;
      recv_size += recvd;

   }

   printf("recieved : %i bytes\n", buffer);
   u32 fb_w, fb_h;
   u16* top_fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &fb_w, &fb_h);

   int x, y;
   u16* rgui_buffer = (u16*)file_buffer;
   for (y = 0; y < 320; y++)
      for (x = 0; x < fb_w; x++)
         top_fb[x + ((y + 40) * fb_w)] = rgui_buffer[y + (fb_w - x - 1)* 320];
//   memcpy(top_fb, file_buffer, file_size);

   free(file_buffer);

   const char* answer_str = "3DS says hi !!";
   send(clientfd, answer_str, strlen(answer_str) + 1, 0);
   close(clientfd);
   close(sockfd);

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

   socExit();
   free(SOC_buffer);

   gfxExit();
   return 0;
}






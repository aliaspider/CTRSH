
#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

#include "commands.h"
#include "ctrsh.h"
#include "ctr_net.h"
#include "ctr_debug.h"


void ctrsh_command_exit(Handle socket, ctrnet_sockaddr_in_t* addr)
{

}

void ctrsh_command_dirent(Handle socket, ctrnet_sockaddr_in_t* addr)
{
   int i;
   FS_Archive sdmc_archive = {0};
   sdmc_archive.id = ARCHIVE_SDMC;
   FSUSER_OpenArchive(&sdmc_archive);
   const wchar_t* dirpath_c = L"/";
   FS_Path dirpath;
   dirpath.type = PATH_UTF16;
   dirpath.size = (wcslen(dirpath_c) + 1) * sizeof(*dirpath_c);
   dirpath.data = dirpath_c;

   _Static_assert(sizeof(*dirpath_c) == 2, "wchar_t");
   Handle dirhandle;
   DEBUG_ERROR(FSUSER_OpenDirectory(&dirhandle, sdmc_archive, dirpath));

   FS_DirectoryEntry dir_entries[256];
   char dir_names[256][0x106] = {0};
   u32 dircount = sizeof(dir_entries) / sizeof(*dir_entries);
   DEBUG_ERROR(FSDIR_Read(dirhandle, &dircount, dircount, dir_entries));

   DEBUG_VAR(dircount);

   for (i = 0; i < dircount; i++)
   {
      utf16_to_utf8(dir_names[i], dir_entries[i].name, 0x105);
      DEBUG_STR(dir_names[i]);
   }


   ctrnet_send(socket, &dircount, sizeof(dircount), 0, addr);

   for (i = 0; i  < dircount; i++)
   {
      u32 dirname_len = strlen(dir_names[i]) + 1;
      ctrnet_send(socket, &dirname_len, 4, 0, addr);
      ctrnet_send(socket, dir_names[i], dirname_len, 0, addr);
   }

}

void ctrsh_command_display_image(Handle socket, ctrnet_sockaddr_in_t* addr)
{
   u8* file_buffer = NULL;
   int total_size = 0;
   u64 start_tick = svcGetSystemTick();

   int file_size = 0;
   while (!file_size)
      ctrnet_recv(socket, &file_size, 4, 0, addr);

   //         DEBUG_VAR(file_size);

   file_buffer = malloc(file_size);

   int recv_size = 0;

   while (recv_size < file_size)
   {
      u32 recvd;
      recvd = ctrnet_recv(socket, file_buffer + recv_size, file_size - recv_size, 0, addr);
      //            DEBUG_VAR(recvd);
      DEBUG_ERROR(recvd);

      if (recvd < 0)
         break;

      recv_size += recvd;

   }

   //         printf("recieved : %i bytes\n", recv_size);
   total_size += recv_size;

   u64 end_tick = svcGetSystemTick();
   printf("total : %i, time: %f\n", total_size, (end_tick - start_tick) / 268123480.0);
   printf("speed: %.3f KB/s", total_size * 268123480.0 / (1024.0 * (end_tick - start_tick)));

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


}

void ctrsh_wait_command(Handle socket, ctrnet_sockaddr_in_t* addr)
{
   u32 command_id = 0;
   while (aptMainLoop())
   {
      DEBUG_ERROR(ctrnet_recv(socket, &command_id, 4, 0, addr));
      if((command_id == CTRSH_COMMAND_INVALID) || (command_id >= CTRSH_COMMAND_ID_MAX))
         continue;

      ctrsh.commands[command_id].fn(socket, addr);
      if(command_id == CTRSH_COMMAND_EXIT)
         break;
   }
}

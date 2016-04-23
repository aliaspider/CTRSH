
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
   const wchar_t* dirpath_c = L"/";
   FS_Path dirpath;
   dirpath.type = PATH_UTF16;
   dirpath.size = (wcslen(dirpath_c) + 1) * sizeof(*dirpath_c);
   dirpath.data = dirpath_c;
   _Static_assert(sizeof(*dirpath_c) == 2, "wchar_t");

   Handle dirhandle;
   DEBUG_ERROR(FSUSER_OpenDirectory(&dirhandle, ctrsh.sdmc, dirpath));
   int i;
   u32 dirent_buffer_size = 0x2000;
   u8* dirent_buffer = malloc(dirent_buffer_size);
   u32 dirent_buffer_offset = 0;
   u32 dir_count = 1;
   while(true)
   {
      FS_DirectoryEntry dir_entries[0x80];
      dir_count = sizeof(dir_entries) / sizeof(*dir_entries);
      DEBUG_ERROR(FSDIR_Read(dirhandle, &dir_count, dir_count, dir_entries));
      if(!dir_count)
         break;
      for(i = 0; i < dir_count; i++)
      {
         if(dirent_buffer_offset + 0x200 > dirent_buffer_size)
         {
            dirent_buffer_size <<= 1;
            dirent_buffer = realloc(dirent_buffer, dirent_buffer_size);
            DEBUG_VAR(dirent_buffer_size);
         }
         ctrsh_dirent* dst = (ctrsh_dirent*)(dirent_buffer + dirent_buffer_offset);
         dst->attributes = dir_entries[i].attributes;
         dst->size = dir_entries[i].fileSize;
         size_t ret = wcstombs((char*)dst->name, dir_entries[i].name, 0x100);
         if(ret == (size_t)-1)
            continue;
         dst->name[ret] = '\0';
         dst->next = 0x10 + ret + 1;
         dirent_buffer_offset += dst->next;

         dirent_buffer_offset = &dst->name[ret] + 1 - dirent_buffer;
         dst->next = &dst->name[ret] + 1 - (u8*)dst;
      }
   }
   DEBUG_ERROR(ctrnet_send(socket, &dirent_buffer_offset, 4, 0, addr));
   DEBUG_ERROR(ctrnet_send(socket, dirent_buffer, dirent_buffer_offset, 0, addr));
   DEBUG_ERROR(svcCloseHandle(dirhandle));
   free(dirent_buffer);
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
   printf("speed: %.3f KB/s\n", total_size * 268123480.0 / (1024.0 * (end_tick - start_tick)));

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
      gfxFlushBuffers();
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

      u64 start_tick = svcGetSystemTick();
      ctrsh.commands[command_id].fn(socket, addr);
      u64 end_tick = svcGetSystemTick();
      printf("<%s> executed in %.3fms\n", ctrsh.commands[command_id].name, (end_tick - start_tick) / 268123.480);

      if(command_id == CTRSH_COMMAND_EXIT)
         break;
   }
}


#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

#include "commands.h"
#include "ctrsh.h"
#include "ctr_net.h"
#include "ctr_debug.h"

static Result ctrsh_send_from_buffer(Handle socket, void* buffer, u32 size, ctrnet_sockaddr_in_t* addr)
{
   Result res;
   u64 start_tick, end_tick;

   start_tick = svcGetSystemTick();

   res = ctrnet_send(socket, &size, 4, 0, addr);

   if (res < 0)
      return res;

   res = ctrnet_send(socket, buffer, size, 0, addr);

   if (res < 0)
      return res;

   end_tick = svcGetSystemTick();

   printf("sent : %i Bytes , time: %f\n", (int)size, (end_tick - start_tick) / 268123480.0);
   printf("speed: %.3f KB/s\n", size * 268123480.0 / (1024.0 * (end_tick - start_tick)));

   return res;

}


static Result ctrsh_recv_to_file(Handle socket, Handle file, ctrnet_sockaddr_in_t* addr)
{
   Result res;
   int file_size, remaining;

   u64 start_tick, end_tick;

   start_tick = svcGetSystemTick();

   res = ctrnet_recv(socket, &file_size, 4, 0, addr);

   if (res < 0)
      return res;

   void* buffer = malloc(CTRSH_FILE_BUFFER_SIZE);

   remaining = file_size;

   while (remaining > 0)
   {
      res = ctrnet_recv(socket, buffer, remaining < CTRSH_FILE_BUFFER_SIZE ? remaining : CTRSH_FILE_BUFFER_SIZE, 0, addr);

      if (res < 0)
         return res;

      remaining -= res;
   }
   free(buffer);

   end_tick = svcGetSystemTick();

   printf("recieved : %i Bytes, time: %f\n", file_size, (end_tick - start_tick) / 268123480.0);
   printf("speed: %.3f KB/s\n", file_size * 268123480.0 / (1024.0 * (end_tick - start_tick)));

   return file_size;
}


static Result ctrsh_recv_to_buffer(Handle socket, void** buffer, ctrnet_sockaddr_in_t* addr)
{
   Result res;
   int file_size, recv_size;
   u64 start_tick, end_tick;

   start_tick = svcGetSystemTick();

   res = ctrnet_recv(socket, &file_size, 4, 0, addr);

   if (res < 0)
      return res;

   *buffer = malloc(file_size);

   recv_size = 0;

   while (recv_size < file_size)
   {
      res = ctrnet_recv(socket, (u8*)*buffer + recv_size, file_size - recv_size, 0, addr);

      if (res < 0)
         return res;

      recv_size += res;
   }

   end_tick = svcGetSystemTick();

   printf("recieved : %i Bytes, time: %f\n", recv_size, (end_tick - start_tick) / 268123480.0);
   printf("speed: %.3f KB/s\n", recv_size * 268123480.0 / (1024.0 * (end_tick - start_tick)));

   return file_size;
}


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

   while (true)
   {
      FS_DirectoryEntry dir_entries[0x80];
      dir_count = sizeof(dir_entries) / sizeof(*dir_entries);
      DEBUG_ERROR(FSDIR_Read(dirhandle, &dir_count, dir_count, dir_entries));

      if (!dir_count)
         break;

      for (i = 0; i < dir_count; i++)
      {
         if (dirent_buffer_offset + 0x200 > dirent_buffer_size)
         {
            dirent_buffer_size <<= 1;
            dirent_buffer = realloc(dirent_buffer, dirent_buffer_size);
            DEBUG_VAR(dirent_buffer_size);
         }

         ctrsh_dirent* dst = (ctrsh_dirent*)(dirent_buffer + dirent_buffer_offset);
         dst->attributes = dir_entries[i].attributes;
         dst->file_size = dir_entries[i].fileSize;
         dst->mbslen = wcslen(dir_entries[i].name);
         size_t ret = wcstombs((char*)dst->name, dir_entries[i].name, 0x100);

         if (ret == (size_t) - 1)
            continue;

         dst->name[ret] = '\0';
         ret += 0x8;
         ret &= ~0x7;
         dirent_buffer_offset = &dst->name[ret] - dirent_buffer;
         dst->entry_size = &dst->name[ret] - (u8*)dst;
      }
   }

   ((ctrsh_dirent*)(dirent_buffer + dirent_buffer_offset))->entry_size = 0;
   dirent_buffer_offset += 4;

//   DEBUG_ERROR(ctrnet_send(socket, &dirent_buffer_offset, 4, 0, addr));
//   DEBUG_ERROR(ctrnet_send(socket, dirent_buffer, dirent_buffer_offset, 0, addr));
   DEBUG_ERROR(ctrsh_send_from_buffer(socket, dirent_buffer, dirent_buffer_offset, addr));
   DEBUG_ERROR(svcCloseHandle(dirhandle));
   free(dirent_buffer);
}

void ctrsh_command_display_image(Handle socket, ctrnet_sockaddr_in_t* addr)
{
   void* file_buffer = NULL;
   ssize_t file_size = ctrsh_recv_to_buffer(socket, &file_buffer, addr);
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


void ctrsh_command_put(Handle socket, ctrnet_sockaddr_in_t* addr)
{
   printf("recieving file ...\n");
   DEBUG_ERROR(ctrsh_recv_to_file(socket, 0, addr));
}


void ctrsh_wait_command(Handle socket, ctrnet_sockaddr_in_t* addr)
{
   u32 command_id = 0;

   while (aptMainLoop())
   {
      DEBUG_ERROR(ctrnet_recv(socket, &command_id, 4, 0, addr));

      if ((command_id == CTRSH_COMMAND_INVALID) || (command_id >= CTRSH_COMMAND_ID_MAX))
         continue;

      u64 start_tick = svcGetSystemTick();
      ctrsh.commands[command_id].fn(socket, addr);
      u64 end_tick = svcGetSystemTick();
      printf("<%s> executed in %.3fms\n", ctrsh.commands[command_id].name, (end_tick - start_tick) / 268123.480);

      if (command_id == CTRSH_COMMAND_EXIT)
         break;
   }
}

#include <stdlib.h>
#include <wchar.h>
#include <3ds/svc.h>
#include <3ds/services/fs.h>

#include "common.h"
#include "commands.h"
#include "server_cmd.h"

#include "ctr/ctr_debug.h"
#include "net/net_ctrl.h"

void command_dirent_entry()
{
   const wchar_t* dirpath_w = L"/";
   wchar_t* dirpath_w_buffer = NULL;
   char* dirpath_c = NULL;
   int dirpath_len = ctrsh_recv_to_buffer((void**)&dirpath_c);
   DEBUG_VAR(dirpath_len);
   DEBUG_ERROR(dirpath_len);
   if(dirpath_len)
   {
      DEBUG_STR(dirpath_c);
      dirpath_w_buffer = malloc((dirpath_len + 1) * sizeof(wchar_t));
      mbstowcs(dirpath_w_buffer, dirpath_c, dirpath_len + 1);
      dirpath_w = dirpath_w_buffer;
   }

   FS_Path dirpath;
   dirpath.type = PATH_UTF16;
   dirpath.size = (wcslen(dirpath_w) + 1) * sizeof(*dirpath_w);
   dirpath.data = dirpath_w;
   _Static_assert(sizeof(*dirpath_w) == 2, "wchar_t");

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
   DEBUG_ERROR(send_from_buffer(dirent_buffer, dirent_buffer_offset));
   DEBUG_ERROR(svcCloseHandle(dirhandle));
   free(dirent_buffer);
   free(dirpath_w_buffer);
}


ctrsh_command_t command_dirent =
{
   CTRSH_COMMAND_DIRENT,
   "dirent",
   command_dirent_entry
};

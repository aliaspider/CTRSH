#include <stdlib.h>
#include <wchar.h>
#include <3ds/svc.h>
#include <3ds/services/fs.h>

#include "common.h"
#include "commands.h"
#include "server_cmd.h"

#include "ctr/ctr_debug.h"
#include "net/net_ctrl.h"
#include "utils/sdmc.h"

void command_dirent_entry()
{
   char* path;
   Handle dirhandle;

   DEBUG_ERROR(ctrsh_recv_to_buffer((void**)&path));
   DEBUG_ERROR(sdmc_open_dir(&dirhandle, path));
   free(path);

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
}


ctrsh_command_t command_dirent =
{
   CTRSH_COMMAND_DIRENT,
   "dirent",
   command_dirent_entry
};

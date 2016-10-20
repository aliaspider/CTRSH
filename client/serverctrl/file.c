
#include "utils/file_list.h"
#include "common.h"
#include "server_ctrl.h"
#include "server_internal.h"

filelist_t* server_get_filelist(const char* path)
{
   DEBUG_ERROR(server_send_command(CTRSH_COMMAND_DIRENT));
   if(path)
      DEBUG_ERROR(server_send_data(path, strlen(path)+ 1));
   else
      DEBUG_ERROR(server_send_int32(0));

   void* dirent_buffer;
   server_recv_data(&dirent_buffer);
   filelist_t* filelist = filelist_new(dirent_buffer);
   filelist_sort(filelist);
   filelist_sort_dir(filelist);

   return filelist;
}

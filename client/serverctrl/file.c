
#include "utils/file_list.h"
#include "common.h"
#include "server_ctrl.h"

filelist_t* server_get_filelist(const char* path)
{
   DEBUG_ERROR(send_command(CTRSH_COMMAND_DIRENT));
   uint32_t buffer_size;
   DEBUG_ERROR(recv_data(&buffer_size, 4));

   if (!buffer_size)
      NULL;

   uint8_t* buffer = malloc(buffer_size);
   ssize_t bytes_read = 0;

   while (bytes_read < buffer_size)
   {
      ssize_t ret = recv_data(buffer + bytes_read, buffer_size - bytes_read);
      DEBUG_ERROR(ret);
      bytes_read += ret;
   }

   filelist_t* filelist = filelist_new((ctrsh_dirent*)buffer);
   filelist_sort(filelist);
   filelist_sort_dir(filelist);

   return filelist;

}

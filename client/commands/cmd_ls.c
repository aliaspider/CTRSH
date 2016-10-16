#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "commands.h"
#include "utils/file_list.h"
#include "../server/server_cmd.h"

void command_ls(int sockfd, int argc, char* const* argv)
{
   int i, j;

   bool detailed_view = false;
   int opt;

   while ((opt = getopt(argc, argv, "lh")) != -1)
   {
      switch (opt)
      {
      case 'l':
         detailed_view = true;
         break;

      case '?':
      case 'h':
         printf("\nUsage: ls [options]\n\n");
         printf("   -l   detailed view\n");
         printf("   -h   print help\n");
         printf("\n");
         return;

      default:
         break;
      }
   }

   DEBUG_ERROR(send_command(sockfd, CTRSH_COMMAND_DIRENT));
   uint32_t buffer_size;
   DEBUG_ERROR(read(sockfd, &buffer_size, 4));

   if (!buffer_size)
      return;

   uint8_t* buffer = malloc(buffer_size);
   ssize_t bytes_read = 0;

   while (bytes_read < buffer_size)
   {
      ssize_t ret = read(sockfd, buffer + bytes_read, buffer_size - bytes_read);
      DEBUG_ERROR(ret);
      bytes_read += ret;
   }

#if 1
   FILE* fp = fopen("dirent_test.bin", "wb");
   fwrite(buffer, 1, buffer_size, fp);
   fclose(fp);
#endif

   filelist_t* filelist = filelist_new((ctrsh_dirent*)buffer);
   filelist_sort(filelist);
   filelist_sort_dir(filelist);

   if (detailed_view)
      filelist_print_detailed(filelist);
   else
      filelist_print(filelist);

   filelist_free(filelist);
   free(buffer);

}


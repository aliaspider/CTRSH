#include <stdlib.h>
#include <stdio.h>

#include <3ds/svc.h>
#include <3ds/types.h>

#include "ctr/ctr_net.h"
#include "ctr/ctr_debug.h"

#include "common.h"
#include "net_ctrl.h"
#include "net_internal.h"

Result recv_to_file(Handle file)
{
   Result res;

   int file_size, remaining;



   profiler_start();

   res = ctrnet_recv(client, &file_size, 4, 0, &client_addr);

   if (res < 0)
      return res;

   void* buffer = malloc(CTRSH_FILE_BUFFER_SIZE);

   remaining = file_size;

   while (remaining > 0)
   {
      res = ctrnet_recv(client, buffer, remaining < CTRSH_FILE_BUFFER_SIZE ? remaining : CTRSH_FILE_BUFFER_SIZE, 0,
                        &client_addr);

      if (res < 0)
         return res;

      remaining -= res;
      printf("recieving\n");
   }

   free(buffer);

   profiler_stop();
   profiler_speed("recieved", file_size);

   return file_size;
}


Result ctrsh_recv_to_buffer(void** buffer)
{
   Result res;

   int file_size, recv_size;
   profiler_start();

   res = ctrnet_recv(client, &file_size, 4, 0, &client_addr);

   if (res < 0)
      return res;

   if(!file_size)
   {
      *buffer = NULL;
      return 0;
   }

   *buffer = malloc(file_size);

   recv_size = 0;

   while (recv_size < file_size)
   {
      res = ctrnet_recv(client, (u8*)*buffer + recv_size, file_size - recv_size, 0, &client_addr);

      if (res < 0)
         return res;

      recv_size += res;
      printf("recieving\n");
   }

   profiler_stop();
   profiler_speed("recieved", recv_size);

   return file_size;
}

Result recv_u32(u32* out)
{
   return ctrnet_recv(client, out, 4, 0, &client_addr);
}

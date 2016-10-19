#include <stdlib.h>
#include <stdio.h>

#include <3ds/svc.h>
#include <3ds/types.h>

#include "ctr/ctr_net.h"
#include "ctr/ctr_debug.h"

#include "common.h"
#include "net_ctrl.h"
#include "net_internal.h"

Result ctrsh_recv_to_file(Handle file)
{
   Result res;
   Handle socket = ctrsh.server.client;
   ctrnet_sockaddr_in_t* addr = &ctrsh.server.client_addr;

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
      printf("recieving\n");
   }
   free(buffer);

   end_tick = svcGetSystemTick();

   printf("recieved : %i Bytes, time: %f\n", file_size, (end_tick - start_tick) / 268123480.0);
   printf("speed: %.3f KB/s\n", file_size * 268123480.0 / (1024.0 * (end_tick - start_tick)));

   return file_size;
}


Result ctrsh_recv_to_buffer(void** buffer)
{
   Result res;
   Handle socket = ctrsh.server.client;
   ctrnet_sockaddr_in_t* addr = &ctrsh.server.client_addr;

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
      printf("recieving\n");
   }

   end_tick = svcGetSystemTick();

   printf("recieved : %i Bytes, time: %f\n", recv_size, (end_tick - start_tick) / 268123480.0);
   printf("speed: %.3f KB/s\n", recv_size * 268123480.0 / (1024.0 * (end_tick - start_tick)));

   return file_size;
}


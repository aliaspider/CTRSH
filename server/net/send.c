#include <stdio.h>

#include <3ds/svc.h>
#include <3ds/types.h>

#include "ctr/ctr_net.h"

#include "common.h"
#include "net_ctrl.h"
#include "net_internal.h"

Result send_from_buffer(void* buffer, u32 size)
{
   Result res;

   u64 start_tick, end_tick;

   start_tick = svcGetSystemTick();

   res = ctrnet_send(client, &size, 4, 0, &client_addr);

   if (res < 0)
      return res;

   res = ctrnet_send(client, buffer, size, 0, &client_addr);

   if (res < 0)
      return res;

   end_tick = svcGetSystemTick();

   printf("sent : %i Bytes , time: %f\n", (int)size, (end_tick - start_tick) / 268123480.0);
   printf("speed: %.3f KB/s\n", size * 268123480.0 / (1024.0 * (end_tick - start_tick)));

   return res;

}

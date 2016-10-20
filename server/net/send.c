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

   profiler_start();

   res = ctrnet_send(client, &size, 4, 0, &client_addr);

   if (res < 0)
      return res;

   res = ctrnet_send(client, buffer, size, 0, &client_addr);

   if (res < 0)
      return res;

   profiler_stop();
   profiler_speed("sent", size);

   return res;

}

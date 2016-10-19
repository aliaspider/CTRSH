
#include <string.h>
#include <3ds/types.h>
#include <3ds/services/apt.h>
#include "ctr/ctr_net.h"
#include "ctr/ctr_debug.h"
#include <arpa/inet.h>

#include "common.h"
#include "net_ctrl.h"
#include "net_internal.h"


void server_init(void)
{
   memset(&ctrsh.server.host_addr, 0, sizeof ctrsh.server.host_addr);
   memset(&ctrsh.server.client_addr, 0, sizeof ctrsh.server.client_addr);

   //   ndmuInit();
   //   ndmuEnterExclusiveState(EXCLUSIVE_STATE_INFRASTRUCTURE);
   //   Result ndmuSuspendDaemons(u32 mask);
   //   Result ndmuResumeDaemons(u32 mask);
   //   Result ndmuSuspendScheduler(bool Async);
   //   Result ndmuResumeScheduler(void);
   //   ndmuSuspendDaemons(0xF);
   //   ndmuSuspendScheduler(false);

   //   ndmuExit();
   DEBUG_ERROR(ctrnet_init(0x100000));

   ctrsh.server.host_addr.size = sizeof(ctrsh.server.host_addr);
   ctrsh.server.host_addr.family = AF_INET;
   ctrsh.server.host_addr.port = htons(12000);
   DEBUG_ERROR(ctrnet_gethostid(&ctrsh.server.host_addr.addr));
   DEBUG_ERROR(ctrnet_socket(&ctrsh.server.socket));

   u32 sockopt_val = CTRSH_SERVER_SNDRCV_BUFFER_SIZE;
   ctrnet_setsockopt(ctrsh.server.socket, SOL_SOCKET, SO_RCVBUF, &sockopt_val, 4);
   ctrnet_setsockopt(ctrsh.server.socket, SOL_SOCKET, SO_SNDBUF, &sockopt_val, 4);

   DEBUG_ERROR(ctrnet_bind(ctrsh.server.socket, &ctrsh.server.host_addr));
   DEBUG_ERROR(ctrnet_listen(ctrsh.server.socket, 2));

   printf("IP %s\n", ctrnet_sa_to_cstr(&ctrsh.server.host_addr));

   do
   {
      Result ret = ctrnet_accept(ctrsh.server.socket, &ctrsh.server.client, &ctrsh.server.client_addr);
      DEBUG_ERROR(ret);
      if (!ret)
         break;
   }
   while (aptMainLoop());

   printf("Connection from %s\n", ctrnet_sa_to_cstr(&ctrsh.server.client_addr));

   netprint_init();
}


void server_deinit(void)
{
   netprint_deinit();

   ctrnet_close(ctrsh.server.client);
   memset(&ctrsh.server.stdout_addr, 0, sizeof(ctrsh.server.stdout_addr));
   ctrnet_close(ctrsh.server.socket);
   ctrnet_exit();
}


#include <string.h>
#include <3ds/types.h>
#include <3ds/services/apt.h>
#include "ctr/ctr_net.h"
#include "ctr/ctr_debug.h"
#include <arpa/inet.h>

#include "common.h"
#include "net_ctrl.h"
#include "net_internal.h"


Handle client;
ctrnet_sockaddr_in_t client_addr;

static Handle server;
static ctrnet_sockaddr_in_t host_addr;


void server_init(void)
{
   memset(&host_addr, 0, sizeof(host_addr));
   memset(&client_addr, 0, sizeof(client_addr));

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

   host_addr.size = sizeof(host_addr);
   host_addr.family = AF_INET;
   host_addr.port = htons(12000);
   DEBUG_ERROR(ctrnet_gethostid(&host_addr.addr));
   DEBUG_ERROR(ctrnet_socket(&server));

   u32 sockopt_val = CTRSH_SERVER_SNDRCV_BUFFER_SIZE;
   ctrnet_setsockopt(server, SOL_SOCKET, SO_RCVBUF, &sockopt_val, 4);
   ctrnet_setsockopt(server, SOL_SOCKET, SO_SNDBUF, &sockopt_val, 4);

   DEBUG_ERROR(ctrnet_bind(server, &host_addr));
   DEBUG_ERROR(ctrnet_listen(server, 2));

   printf("IP %s\n", ctrnet_sa_to_cstr(&host_addr));

   do
   {
      Result ret = ctrnet_accept(server, &client, &client_addr);
      DEBUG_ERROR(ret);

      if (!ret)
         break;
   }
   while (aptMainLoop());

   printf("Connection from %s\n", ctrnet_sa_to_cstr(&client_addr));

   netprint_init(server);
}


void server_exit(void)
{
   netprint_deinit();

   ctrnet_close(client);
   ctrnet_close(server);
   ctrnet_exit();


   //   ndmuInit();
   //   ndmuLeaveExclusiveState();
   //   ndmuResumeScheduler();
   //   ndmuResumeDaemons(0xF);
   //   ndmuExit();
}

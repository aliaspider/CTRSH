#ifndef COMMON_H
#define COMMON_H

#include <3ds/types.h>
#include <3ds/services/fs.h>

#include "ctr/ctr_net.h"

typedef struct
{
   struct
   {
      Handle socket;
      Handle client;
      Handle stdout_soc;
      ctrnet_sockaddr_in_t host_addr;
      ctrnet_sockaddr_in_t client_addr;
      ctrnet_sockaddr_in_t stdout_addr;
   }server;
   FS_Archive sdmc;
}ctrsh_t;

extern ctrsh_t ctrsh;

#endif // COMMON_H

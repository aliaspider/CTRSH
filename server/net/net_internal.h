#ifndef NET_INTERNAL_H
#define NET_INTERNAL_H

#include <3ds/types.h>
#include "ctr/ctr_net.h"

#define CTRSH_SERVER_SNDRCV_BUFFER_SIZE 0x8000
#define CTRSH_FILE_BUFFER_SIZE 0x8000

extern Handle client;
extern ctrnet_sockaddr_in_t client_addr;

void netprint_init(Handle socket);
void netprint_deinit(void);

#endif // NET_INTERNAL_H

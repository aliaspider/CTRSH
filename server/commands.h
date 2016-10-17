#ifndef COMMANDS_H
#define COMMANDS_H

#include <3ds/types.h>
#include "server_cmd.h"
#include "ctr/ctr_net.h"

typedef void (*ctrsh_command_fn_t)(Handle socket, ctrnet_sockaddr_in_t* addr);

typedef struct
{
   const char* name;
   ctrsh_command_fn_t fn;
}ctrsh_command_t;



void ctrsh_wait_command(Handle socket, ctrnet_sockaddr_in_t* addr);

void ctrsh_command_exit(Handle socket, ctrnet_sockaddr_in_t* addr);
void ctrsh_command_dirent(Handle socket, ctrnet_sockaddr_in_t* addr);
void ctrsh_command_display_image(Handle socket, ctrnet_sockaddr_in_t* addr);
void ctrsh_command_put(Handle socket, ctrnet_sockaddr_in_t* addr);



#endif // COMMANDS_H

#ifndef SERVER_CMD_INTERNAL_H
#define SERVER_CMD_INTERNAL_H

#include <unistd.h>
#include "common.h"
#include "../../server/server_cmd.h"

static inline int send_data(const void *buffer, size_t size)
{
   return write(ctrsh.server.soc, buffer, size);
}

static inline int recv_data(void *buffer, size_t size)
{
   return read(ctrsh.server.soc, buffer, size);
}

static inline int send_command(ctrsh_command_id_t command_id)
{
   uint32_t id = command_id;
   return send_data(&id, 4);
}


#endif // __SERVER_CMD_INTERNAL_H

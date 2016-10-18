#ifndef SERVER_CTRL_H
#define SERVER_CTRL_H

#include <stdint.h>
#include <unistd.h>
#include "utils/file_list.h"
#include "../server/server_cmd.h"
#include "common.h"

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

void server_display_image(const void* buffer, uint32_t size);

filelist_t *server_get_filelist(const char* path);


#endif // SERVER_CTRL_H

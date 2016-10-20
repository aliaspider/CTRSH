
#include <unistd.h>
#include <sys/socket.h>

#include "common.h"
#include "../../server/server_cmd.h"


static inline ssize_t server_send(const void *buffer, size_t size)
{
   return send(ctrsh.server.soc, buffer, size, 0);
}

ssize_t server_send_int32(int32_t val)
{
   return server_send(&val, 4);
}

ssize_t server_send_command(ctrsh_command_id_t command_id)
{
   return server_send_int32(command_id);
}

ssize_t server_send_data(const void *buffer, size_t size)
{
   ssize_t res = server_send_int32(size);
   if(res < 0)
      return res;


   return server_send(buffer, size);
}

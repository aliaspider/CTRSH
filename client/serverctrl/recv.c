
#include <unistd.h>
#include <sys/socket.h>

#include <assert.h>

#include "common.h"

static inline ssize_t server_recv(void* buffer, size_t size)
{
   return recv(ctrsh.server.soc, buffer, size, 0);
}

static inline ssize_t server_recv_int32(int32_t* out)
{
   return server_recv(out, 4);
}

static inline ssize_t server_recv_all(void* buffer, size_t size)
{
   ssize_t bytes_read = 0;

   while (bytes_read < size)
   {
      ssize_t ret = server_recv(buffer + bytes_read, size - bytes_read);

      if (ret < 0)
         return ret;

      bytes_read += ret;
   }

   assert(bytes_read == size);

   return size;
}



ssize_t server_recv_data(void** buffer)
{
   int32_t size;
   ssize_t ret = server_recv_int32(&size);

   if (ret < 0)
      return ret;

   if (!size)
      return 0;

   *buffer = malloc(size);

   return server_recv_all(*buffer, size);
}

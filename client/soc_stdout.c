
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdarg.h>
#include <stdbool.h>

#include "../server/server_cmd.h"
#include "common.h"
#include "utils/file_list.h"

int rl_printf_server(const char* fmt, ...)
{
   va_list va;
   va_start(va, fmt);
   rl_vprintf_ex(KGRY, "[Server] ", fmt, va);
   va_end(va);
}

int rl_printf_server_info(const char* fmt, ...)
{
   va_list va;
   va_start(va, fmt);
   rl_vprintf_color(KLGR, fmt, va);
   va_end(va);
}


bool stdout_thread_running;
void* stdout_thread_entry(void* args)
{
   struct sockaddr* serv_addr = args;
   ctrsh.server.soc_stdout = socket(AF_INET, SOCK_STREAM, 0);
   int sockopt_val = 0x40000;
   setsockopt(ctrsh.server.soc_stdout, SOL_SOCKET, SO_SNDBUF, &sockopt_val, 4);
   setsockopt(ctrsh.server.soc_stdout, SOL_SOCKET, SO_RCVBUF, &sockopt_val, 4);
   int ret;

   do
   {
      ret = connect(ctrsh.server.soc_stdout, serv_addr, sizeof(struct sockaddr_in));

      if (ret < 0)
         usleep(10000);
   }
   while (ret < 0);

   rl_printf_server_info("stdout thread connected\n");


   while(stdout_thread_running)
   {
      char buffer[4096];
      int ret = recv(ctrsh.server.soc_stdout, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
      if( ret < 0)
      {
         if((errno != EAGAIN) && (errno != EWOULDBLOCK))
            break;
         usleep(100000);
      }
      else if(ret)
      {
         buffer[ret] = 0;
         rl_printf_server(buffer);
      }

   }

   close(ctrsh.server.soc_stdout);

   rl_printf_info("stdout thread disconnected\n");

   return NULL;
}

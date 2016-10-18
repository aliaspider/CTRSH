#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <pthread.h>

#include "server_ctrl.h"
#include "server_internal.h"

static int rl_printf_server(const char* fmt, ...)
{
   va_list va;
   va_start(va, fmt);
   rl_vprintf_ex(KGRY, "[Server] ", fmt, va);
   va_end(va);
}

static int rl_printf_server_info(const char* fmt, ...)
{
   va_list va;
   va_start(va, fmt);
   rl_vprintf_color(KLGR, fmt, va);
   va_end(va);
}


static void* stdout_thread_entry(void* args)
{
   ctrsh.server.soc_stdout = socket(AF_INET, SOCK_STREAM, 0);
   int sockopt_val = 0x40000;
   setsockopt(ctrsh.server.soc_stdout, SOL_SOCKET, SO_SNDBUF, &sockopt_val, 4);
   setsockopt(ctrsh.server.soc_stdout, SOL_SOCKET, SO_RCVBUF, &sockopt_val, 4);
   int ret;

   do
   {
      ret = connect(ctrsh.server.soc_stdout, (struct sockaddr*)&ctrsh.server.addr, sizeof(struct sockaddr_in));

      if (ret < 0)
         usleep(10000);
   }
   while (ret < 0);

   rl_printf_server_info("stdout thread connected\n");

   while(ctrsh.server.connected)
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

   rl_printf_info("stdout thread disconnected\n");

   return NULL;
}

void server_connect(void)
{
   ctrsh.server.connected = true;
   memset(&ctrsh.server.addr, 0, sizeof(struct sockaddr_in));
   DEBUG_ERROR(ctrsh.server.soc = socket(AF_INET, SOCK_STREAM, 0));
   int sockopt_val = 0x40000;
   setsockopt(ctrsh.server.soc, SOL_SOCKET, SO_SNDBUF, &sockopt_val, 4);
   setsockopt(ctrsh.server.soc, SOL_SOCKET, SO_RCVBUF, &sockopt_val, 4);

   ctrsh.server.addr.sin_family = AF_INET;
   ctrsh.server.addr.sin_addr.s_addr = ctrsh.server.ip;
   ctrsh.server.addr.sin_port = htons(ctrsh.server.port);
   int ret;

   rl_printf_info("connecting to %i.%i.%i.%i:%i\n", ((uint8_t*)&ctrsh.server.ip)[0], ((uint8_t*)&ctrsh.server.ip)[1],
          ((uint8_t*)&ctrsh.server.ip)[2], ((uint8_t*)&ctrsh.server.ip)[3], ctrsh.server.port);

   do
   {
      ret = connect(ctrsh.server.soc, (struct sockaddr*)&ctrsh.server.addr, sizeof(ctrsh.server.addr));

      if (ret < 0)
         usleep(10000);
   }
   while (ret < 0);


   pthread_create(&ctrsh.stdout_thread, NULL, stdout_thread_entry, NULL);

}

static void server_disconnect(void)
{
   ctrsh.server.connected = false;
   pthread_join(ctrsh.stdout_thread, NULL);
   close(ctrsh.server.soc_stdout);
   close(ctrsh.server.soc);
}

void server_exit(void)
{
   DEBUG_ERROR(send_command(CTRSH_COMMAND_EXIT));
   server_disconnect();
}


void server_put(const void* buffer, uint32_t size)
{
   DEBUG_ERROR(send_command(CTRSH_COMMAND_PUT));
   DEBUG_ERROR(send_data(&size, 4));
   DEBUG_ERROR(send_data(buffer, size));

}

void run_server_3dsx(const char* _3dsx_path)
{
   char _3dslink_cmd[PATH_MAX];

   snprintf(_3dslink_cmd, sizeof(_3dslink_cmd), "3dslink -a %d.%d.%d.%d %s",
            ((uint8_t*)&ctrsh.server.ip)[0], ((uint8_t*)&ctrsh.server.ip)[1],
            ((uint8_t*)&ctrsh.server.ip)[2], ((uint8_t*)&ctrsh.server.ip)[3],
         _3dsx_path);

   while (system(_3dslink_cmd))
      sleep(1);

}

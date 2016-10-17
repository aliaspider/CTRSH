
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


#define IP2INT(a, b, c, d) (a | (b << 8) | (c << 16) | (d <<24))
#define DEBUG_ERROR(X) do{int res_ = (int)(intptr_t)(X); if(res_ < 0){printf("error %i @%s (%s:%d).\n%s\n", res_, __FUNCTION__, __FILE__, __LINE__,strerror(errno)); exit(0);}}while(0)
#define DEBUG_ERROR_stay(X) do{int res_ = (int)(intptr_t)(X); if(res_ < 0){printf("error %i @%s (%s:%d).\n%s\n", res_, __FUNCTION__, __FILE__, __LINE__,strerror(errno));}}while(0)
#define DEBUG_VAR(X) printf( #X" : 0x%08lX\n", (uint32_t)(X))

//#define CTR_IP          IP2INT(10, 42, 0, 237)
#define CTR_IP          IP2INT(192, 168, 2, 240)
#define CTR_PORT        12000
#define HISTORY_FILE    ".ctrsh.hist"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KGRY  "\x1B[90m"
#define KLRD  "\x1B[91m"
#define KLGR  "\x1B[92m"
#define KLYL  "\x1B[93m"
#define KLBL  "\x1B[94m"
#define KLPR  "\x1B[95m"
#define KTRQ  "\x1B[96m"



bool stdout_thread_running;
void* stdout_thread_entry(void* args)
{
   struct sockaddr* serv_addr = args;
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   int sockopt_val = 0x40000;
   setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sockopt_val, 4);
   setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &sockopt_val, 4);
   int ret;

   do
   {
      ret = connect(sockfd, serv_addr, sizeof(struct sockaddr_in));

      if (ret < 0)
         usleep(10000);
   }
   while (ret < 0);

   rl_printf_ex(KLBL, NULL, "stdout thread connected\n");


   while(stdout_thread_running)
   {
      char buffer[4096];
      int ret = recv(sockfd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
      if( ret < 0)
      {
         if((errno != EAGAIN) && (errno != EWOULDBLOCK))
            break;
         usleep(100000);
      }
      else
      {
         buffer[ret] = 0;
         rl_printf_ex(KLBL, "[Server] ", buffer);
      }

   }

   close(sockfd);

   rl_printf_ex(KLBL, NULL, "stdout thread connected\n");

   return NULL;
}

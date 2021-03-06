#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>


#define IP2INT(a, b, c, d) (a | (b << 8) | (c << 16) | (d <<24))
#define DEBUG_ERROR(X) do{int res_ = (int)(intptr_t)(X); if(res_ < 0){rl_printf_error("error %i @%s (%s:%d).\n%s\n", res_, __FUNCTION__, __FILE__, __LINE__,strerror(errno)); exit(0);}}while(0)
#define DEBUG_ERROR_stay(X) do{int res_ = (int)(intptr_t)(X); if(res_ < 0){rl_printf_error("error %i @%s (%s:%d).\n%s\n", res_, __FUNCTION__, __FILE__, __LINE__,strerror(errno));}}while(0)
#define DEBUG_VAR(X) rl_printf_info( #X" : 0x%08lX\n", (uint32_t)(X))


#define KNRM  "\x1B[0m"
#define KBLD  "\x1B[1m"
#define KUDL  "\x1B[4m"
#define KFLA  "\x1B[5m"
#define KREV  "\x1B[7m"
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


typedef struct
{
   char history_file[PATH_MAX];
   bool running;   
   pthread_t stdout_thread;
   struct
   {
      struct sockaddr_in addr;
      bool connected;
      int ip;
      int port;
      int soc;
      int soc_stdout;
   }server;

   struct
   {
      struct
      {
         const char* error;
         const char* info;
#ifndef NDEBUG
         const char* debug;
#endif
      }colors;
   }console;
}ctrsh_t;

extern ctrsh_t ctrsh;

void rl_printf(const char* fmt, ...);
void rl_printf_info(const char* fmt, ...);
void rl_printf_error(const char* fmt, ...);
void rl_printf_debug(const char* fmt, ...);
void rl_printf_ex(const char* color, const char* prefix, const char* fmt, ...);

void rl_vprintf(const char* fmt, va_list va);
void rl_vprintf_color(const char* color, const char* fmt, va_list va);
void rl_vprintf_ex(const char* color, const char* prefix, const char* fmt, va_list va);

extern bool stdout_thread_running;

#endif // COMMON_H


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
#include <getopt.h>
#include <pthread.h>

#include "common.h"
#include "commands.h"
#include "utils/file_list.h"

#define CTR_IP          IP2INT(10, 42, 0, 237)
//#define CTR_IP          IP2INT(192, 168, 2, 240)
#define CTR_PORT        12000
#define HISTORY_FILE    ".ctrsh.hist"

ctrsh_t ctrsh;

typedef struct
{
   ctrsh_dirent** entries;
   int count;
} ctrsh_dirent_list_t;


char* ctrsh_completion_commands(const char* text, int id)
{
   static int index = 0;

   if (id == 0)
      index = 0;

   while (ctrsh_commands[index].name)
   {
      if (!strncmp(text, ctrsh_commands[index].name, strlen(text)))
         return strdup(ctrsh_commands[index++].name);

      index++;
   }

   return NULL;
}

static char** ctrsh_completion_function(const char* str, int start, int end)
{
   int i;
   (void)end;
   for(i = 0; i < start; i++)
      if(!isspace(rl_line_buffer[i]))
         return rl_completion_matches(str, rl_filename_completion_function);

   return rl_completion_matches(str, ctrsh_completion_commands);

}


option_t main_options_defs[] =
{
   {'a', true,  "address", "server address"},
   {'p', true,  "port",    "server port"},
   {'s', true,  "history", "set history file"},
   {'x', true,  "3dsx",    "3dsx file to send with 3dslink before connecting (optional)"},
   {'v', false, "version", "print version"},
   {0}
};

typedef struct
{
   const char* address;
   const char* port;
   const char* history_file;
   const char* _3dsx;
   const char* version;
}main_options_t;

int main(int argc, char* argv[])
{
   opterr = 0;

   ctrsh.history_file[0] = '\0';
   ctrsh.server_ip = CTR_IP;
   ctrsh.server_port = CTR_PORT;
   ctrsh.color_info = KNRM;
   ctrsh.color_error = KLRD;
#ifndef NDEBUG
   ctrsh.color_debug = KMAG;
#endif


   main_options_t* opts = (main_options_t*) parse_options(argc, argv,  main_options_defs);

   if(!opts)
      return 0;

   ctrsh.color_info = KLBL;

   if(opts->version)
   {
      printf("CTRSH version 0.01\n");
      return 0;
   }

   if(opts->history_file)
      strncpy(ctrsh.history_file, opts->history_file, sizeof(ctrsh.history_file));

   if(opts->address)
   {
      ctrsh.server_ip = inet_addr(opts->address);

      if (ctrsh.server_ip == -1)
      {
         rl_printf_error("invalid ip : %s\n", opts->address);
         return 1;
      }
   }
   if(opts->port)
   {
      ctrsh.server_port = atoi(optarg);

      if (ctrsh.server_port  > 0xFFFF)
      {
         rl_printf_error("invalid port number : %s\n", optarg);
         return 1;
      }
   }

   const char* home_path = getenv("HOME");


   if (!ctrsh.history_file[0])
   {
      if (home_path)
         snprintf(ctrsh.history_file, sizeof(ctrsh.history_file), "%s/" HISTORY_FILE, home_path);
      else
         strncpy(ctrsh.history_file, HISTORY_FILE, sizeof(ctrsh.history_file));
   }

   read_history(ctrsh.history_file);
   history_set_pos(history_length);


   rl_attempted_completion_function = ctrsh_completion_function;

//   rl_set_prompt("test:/> ");
   stdout_thread_running = true;

   //    rl_callback_handler_install("", cli_handler);
   //    rl_basic_word_break_characters = "\t\n ";
   //    rl_completer_word_break_characters = "\t\n ";

   if(opts->_3dsx)
   {
      char _3dslink_cmd[PATH_MAX];

      snprintf(_3dslink_cmd, sizeof(_3dslink_cmd), "3dslink -a %d.%d.%d.%d %s",
               ((uint8_t*)&ctrsh.server_ip)[0], ((uint8_t*)&ctrsh.server_ip)[1],
               ((uint8_t*)&ctrsh.server_ip)[2], ((uint8_t*)&ctrsh.server_ip)[3],
            opts->_3dsx);
      while (system(_3dslink_cmd))
         sleep(1);
   }

   ctrsh.running = true;

   while (ctrsh.running)
   {
      ctrsh.server_running = true;
      int sockfd,  n;
      struct sockaddr_in serv_addr = {0};
      DEBUG_ERROR(sockfd = socket(AF_INET, SOCK_STREAM, 0));
      int sockopt_val = 0x40000;
      setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sockopt_val, 4);
      setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &sockopt_val, 4);

      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = ctrsh.server_ip;
      serv_addr.sin_port = htons(ctrsh.server_port);
      int ret;

      rl_printf_info("connecting to %i.%i.%i.%i:%i\n", ((uint8_t*)&ctrsh.server_ip)[0], ((uint8_t*)&ctrsh.server_ip)[1],
             ((uint8_t*)&ctrsh.server_ip)[2], ((uint8_t*)&ctrsh.server_ip)[3], ctrsh.server_port);

      do
      {
         ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

         //          DEBUG_ERROR_stay(ret);
         if (ret < 0)
            usleep(10000);
      }
      while (ret < 0);
//      rl_printf_info("connected to %i.%i.%i.%i:%i\n", ((uint8_t*)&serv_addr.sin_addr.s_addr)[0], ((uint8_t*)&serv_addr.sin_addr.s_addr)[1],
//             ((uint8_t*)&serv_addr.sin_addr.s_addr)[2], ((uint8_t*)&serv_addr.sin_addr.s_addr)[3],  ntohs(serv_addr.sin_port));

      pthread_t stdout_thread;
      pthread_create(&stdout_thread, NULL, stdout_thread_entry, &serv_addr);

      while (ctrsh.server_running)
      {
         char* line_buffer = readline("test:/> ");
         char* line = line_buffer;
         char* last = line;

         while (*line)
         {
            if (!isspace(*line))
               last = line + 1;

            line++;
         }

         *last = '\0';

         line = line_buffer;

         while (isspace(*line))
            line++;

         if (line && *line)
         {
            add_history(line);
            rl_printf_debug("executing command : %s\n", line);
            execute_command(sockfd, line);
         }
         free(line_buffer);
      }

      stdout_thread_running = false;
      pthread_join(stdout_thread, NULL);
      close(sockfd);
      sleep(1);
   }

   write_history(ctrsh.history_file);

   free(opts);
   return 0;
}

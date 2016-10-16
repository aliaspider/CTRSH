
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
   for(i = 0; i < start; i++)
      if(!isspace(rl_line_buffer[i]))
         return rl_completion_matches(str, rl_filename_completion_function);

   return rl_completion_matches(str, ctrsh_completion_commands);

}

static void ctrsh_print_help(void)
{
   printf("\nUsage: ctrsh [options]\n\n");
   printf("   -a   address\n");
   printf("   -p   port\n");
   printf("   -v   print version\n");
   printf("   -h   print help\n");
   printf("\n");
}


static void ctrsh_print_version(void)
{
   printf("CTRSH version 0.01\n");
}

int main(int argc, char* argv[])
{

   int opt;

   ctrsh.history_file[0] = '\0';
   ctrsh.server_ip = CTR_IP;
   ctrsh.server_port = CTR_PORT;

   while ((opt = getopt(argc, argv, "hvsa:p:")) != -1)
   {
      switch (opt)
      {
      case 'v':
         ctrsh_print_version();
         return 0;

      case 's':
         strncpy(ctrsh.history_file, optarg, sizeof(ctrsh.history_file));
         break;

      case 'a':
      {
         ctrsh.server_ip = inet_addr(optarg);

         if (ctrsh.server_ip == -1)
         {
            printf("invalid ip : %s\n", optarg);
            return 1;
         }

         break;
      }

      case 'p':
         ctrsh.server_port = atoi(optarg);

         if (ctrsh.server_port  > 0xFFFF)
         {
            printf("invalid port number : %s\n", optarg);
            return 1;
         }

         break;

      default:
      case '?':
      case 'h':
         ctrsh_print_help();
         return 0;
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

   //    rl_callback_handler_install("", cli_handler);
   //    rl_basic_word_break_characters = "\t\n ";
   //    rl_completer_word_break_characters = "\t\n ";
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

      printf("connecting to %i.%i.%i.%i:%i\n", ((uint8_t*)&ctrsh.server_ip)[0], ((uint8_t*)&ctrsh.server_ip)[1],
             ((uint8_t*)&ctrsh.server_ip)[2], ((uint8_t*)&ctrsh.server_ip)[3], ctrsh.server_port);

      do
      {
         ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

         //          DEBUG_ERROR_stay(ret);
         if (ret < 0)
            usleep(10000);
      }
      while (ret < 0);
//      printf("connected to %i.%i.%i.%i:%i\n", ((uint8_t*)&serv_addr.sin_addr.s_addr)[0], ((uint8_t*)&serv_addr.sin_addr.s_addr)[1],
//             ((uint8_t*)&serv_addr.sin_addr.s_addr)[2], ((uint8_t*)&serv_addr.sin_addr.s_addr)[3],  ntohs(serv_addr.sin_port));

      stdout_thread_running = true;
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
            add_history(line);

         printf("executing command : %s\n", line);
         command_t* cmd = ctrsh_commands;

         while (cmd->name)
         {
            if (!strncmp(line, cmd->name, strlen(cmd->name)))
            {
               int cmd_argc = 1;
               char* ptr = line;

               while (*ptr)
               {
                  if (isspace(*ptr))
                  {
                     *ptr = '\0';
                     ptr++;

                     while (isspace(*ptr))
                        ptr++;

                     if (*ptr)
                        cmd_argc++;
                  }

                  ptr++;
               }

               int idx;
               char* cmd_argv[cmd_argc + 1];
               ptr = line;

               for (idx = 0; idx < cmd_argc; idx++)
               {
                  while (isspace(*ptr))
                     ptr++;

                  cmd_argv[idx] = ptr;

                  while (*ptr)
                     ptr++;

                  while (!*ptr)
                     ptr++;
               }

               cmd_argv[cmd_argc] = NULL;
               optind = 1;
               optopt = 0;
               optarg = NULL;

               cmd->fn(sockfd, cmd_argc, cmd_argv);
               break;
            }

            cmd++;
         }

         free(line_buffer);
      }

      stdout_thread_running = false;
      pthread_join(stdout_thread, NULL);
      close(sockfd);
      sleep(1);
   }

   write_history(ctrsh.history_file);

   return 0;
}

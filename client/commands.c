#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "commands.h"
#include "common.h"

extern option_t command_ls_options[];

void command_exit(int sockfd, char* const* options);
void command_ls(int sockfd, char* const* options);
void command_put(int sockfd, char* const* options);
void command_quit(int sockfd, char* const* options);
void command_send(int sockfd, char* const* options);

command_t ctrsh_commands[] =
{
   {"exit", command_exit, NULL},
   {"ls", command_ls, command_ls_options},
   {"put", command_put, NULL},
   {"quit", command_quit, NULL},
   {"send", command_send, NULL},
   {NULL}
};

void execute_command(int sockfd, char* line)
{
   command_t* cmd = ctrsh_commands;

   while (cmd->name)
   {
      if (!strncmp(line, cmd->name, strlen(cmd->name)))
      {
         int cmd_argc = 0;
         char* ptr = line;

         while (*ptr)
         {
            while (isspace(*ptr))
               ptr++;

            if (*ptr)
            {
               cmd_argc++;
               while (*ptr && !isspace(*ptr))
                  ptr++;
            }
         }


         int idx;
         char* cmd_argv[cmd_argc + 1];
         ptr = line;

         for (idx = 0; idx < cmd_argc; idx++)
         {
            while (isspace(*ptr))
               ptr++;

            cmd_argv[idx] = ptr;

            while (*ptr && !isspace(*ptr))
               ptr++;

            *ptr++ = '\0';
         }

         cmd_argv[cmd_argc] = NULL;
         char **vals = parse_options(cmd_argc, cmd_argv, cmd->options);
         if(vals)
         {
            cmd->fn(sockfd, vals);
            free(vals);
         }
         return;
      }

      cmd++;
   }

   char* ptr = line;
   while (*ptr && !isspace(*ptr))
      ptr++;
   *ptr = 0;
   rl_printf_error("unknown command : %s\n", line);

}

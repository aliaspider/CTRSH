#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "commands.h"
#include "common.h"

extern command_t command_exit;
extern command_t command_ls;
extern command_t command_put;
extern command_t command_quit;
extern command_t command_send;

command_t* ctrsh_commands[] =
{
   &command_exit,
   &command_ls,
   &command_put,
   &command_quit,
   &command_send,
   NULL
};

void execute_command(char* line)
{
   command_t** cmd = ctrsh_commands;

   while (*cmd)
   {
      if (!strncmp(line, (*cmd)->name, strlen((*cmd)->name)))
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
         char **vals = parse_options(cmd_argc, cmd_argv, (*cmd)->options);
         if(vals)
         {
            (*cmd)->fn(vals);
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


char* ctrsh_completion_commands(const char* text, int id)
{
   static int index = 0;

   if (id == 0)
      index = 0;

   while (ctrsh_commands[index])
   {
      if (!strncmp(text, ctrsh_commands[index]->name, strlen(text)))
         return strdup(ctrsh_commands[index++]->name);

      index++;
   }

   return NULL;
}
